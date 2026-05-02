package com.elleann.android.ui.shneditor

/*══════════════════════════════════════════════════════════════════════════════
 * SHNScreen.kt — SHN file editor screen for Elle-Ann Android app.
 *
 *   Ported from SHNDecryptor (C#) by Josh. Reads, edits, and saves Fiesta
 *   Online .shn binary data table files. Files can be opened from device
 *   storage or fetched from the Elle server, and saved back to either.
 *
 *   SHN format:
 *     [0x20 bytes]  CryptHeader
 *     [4 bytes]     Total encrypted length (including this field + header)
 *     [N bytes]     Encrypted payload
 *
 *   Decrypt algo (same XOR as original C# Decrypt()):
 *     Walk backwards from end, XOR each byte with a rolling key derived from
 *     position and length. Encryption == Decryption (symmetric).
 *
 *   After decrypt, payload layout:
 *     uint32  Header
 *     uint32  RecordCount
 *     uint32  DefaultRecordLength
 *     uint32  ColumnCount
 *     for each column:
 *       char[0x30]  Name (null-terminated, zero-padded)
 *       uint32      Type
 *       int32       Length
 *     for each record:
 *       uint16      Row length prefix
 *       [columns]   Binary-packed column data
 *
 *   Column types:
 *     1,12,0x10 → Byte     2 → UShort    3,11,0x12,0x1b → UInt
 *     5 → Float            13,0x15 → Short              0x16 → Int
 *     20 → SByte           9,0x18 → fixed-len String    0x1a → null-term String
 *══════════════════════════════════════════════════════════════════════════════*/

import android.net.Uri
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.rounded.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.compose.viewModel
import com.elleann.android.ui.components.*
import com.elleann.android.ui.theme.*
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.ByteArrayOutputStream
import java.io.InputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder

// ─── Data model ──────────────────────────────────────────────────────────────

data class SHNColumn(
    val name:   String,
    val type:   UInt,
    val length: Int,
)

data class SHNFile(
    val cryptHeader:         ByteArray      = ByteArray(0x20),
    val header:              UInt           = 0u,
    val columns:             List<SHNColumn> = emptyList(),
    val rows:                List<List<Any>> = emptyList(),
    val defaultRecordLength: UInt           = 0u,
)

// ─── State ───────────────────────────────────────────────────────────────────

data class SHNState(
    val file:       SHNFile?    = null,
    val fileName:   String      = "",
    val loading:    Boolean     = false,
    val saving:     Boolean     = false,
    val error:      String?     = null,
    val dirtyRows:  Set<Int>    = emptySet(),   // row indices with unsaved edits
    val search:     String      = "",
    val filterCol:  Int         = 0,
    val saved:      Boolean     = false,
)

// ─── Decrypt / Encrypt (symmetric) ───────────────────────────────────────────

private fun shnCrypt(data: ByteArray) {
    if (data.isEmpty()) return
    var key = (data.size and 0xFF).toByte()
    for (i in data.indices.reversed()) {
        data[i] = (data[i].toInt() xor key.toInt()).toByte()
        var k = (i and 15).toByte()
        k = (k + 0x55).toByte()
        k = (k.toInt() xor ((i * 11) and 0xFF)).toByte()
        k = (k.toInt() xor key.toInt()).toByte()
        k = (k.toInt() xor 170).toByte()
        key = k
    }
}

// ─── Binary reader helpers ────────────────────────────────────────────────────

private fun ByteBuffer.readFixedString(len: Int): String {
    val bytes = ByteArray(len)
    get(bytes)
    val nullIdx = bytes.indexOf(0)
    val end = if (nullIdx >= 0) nullIdx else len
    return String(bytes, 0, end, Charsets.ISO_8859_1)
}

private fun ByteBuffer.readNullTermString(): String {
    val buf = mutableListOf<Byte>()
    while (hasRemaining()) {
        val b = get()
        if (b == 0.toByte()) break
        buf.add(b)
    }
    return String(buf.toByteArray(), Charsets.ISO_8859_1)
}

// ─── Parse ───────────────────────────────────────────────────────────────────

private fun parseSHN(raw: ByteArray): SHNFile {
    if (raw.size < 0x28) throw IllegalArgumentException("File too small")

    val cryptHeader = raw.copyOf(0x20)
    val totalLen    = ByteBuffer.wrap(raw, 0x20, 4).order(ByteOrder.LITTLE_ENDIAN).int
    val payloadLen  = totalLen - 0x24
    if (payloadLen <= 0 || raw.size < 0x24 + payloadLen)
        throw IllegalArgumentException("Payload length mismatch")

    val payload = raw.copyOfRange(0x24, 0x24 + payloadLen)
    shnCrypt(payload)

    val bb = ByteBuffer.wrap(payload).order(ByteOrder.LITTLE_ENDIAN)

    val fileHeader    = bb.int.toUInt()
    val recordCount   = bb.int.toUInt()
    val defRecLen     = bb.int.toUInt()
    val columnCount   = bb.int

    val columns = (0 until columnCount).map {
        var name = bb.readFixedString(0x30)
        if (name.isBlank()) name = "UnkCol$it"
        val type = bb.int.toUInt()
        val len  = bb.int
        SHNColumn(name, type, len)
    }

    val rows = (0u until recordCount).map {
        bb.short // row length prefix — skip
        columns.map { col ->
            when (col.type.toInt()) {
                1, 12, 0x10       -> bb.get().toUByte().toLong()
                2                 -> bb.short.toUShort().toLong()
                3, 11, 0x12, 0x1b -> bb.int.toUInt().toLong()
                5                 -> bb.float.toDouble()
                13, 0x15          -> bb.short.toLong()
                0x16              -> bb.int.toLong()
                20                -> bb.get().toLong()
                9, 0x18           -> bb.readFixedString(col.length)
                0x1a              -> bb.readNullTermString()
                else              -> bb.get().toLong()
            }
        }
    }

    return SHNFile(
        cryptHeader         = cryptHeader,
        header              = fileHeader,
        columns             = columns,
        rows                = rows,
        defaultRecordLength = defRecLen,
    )
}

// ─── Serialize back to SHN bytes ─────────────────────────────────────────────

private fun serializeSHN(file: SHNFile): ByteArray {
    val out = ByteArrayOutputStream()
    fun writeLE32(v: Int)    { out.write(byteArrayOf(v.toByte(), (v shr 8).toByte(), (v shr 16).toByte(), (v shr 24).toByte())) }
    fun writeLE16(v: Int)    { out.write(byteArrayOf(v.toByte(), (v shr 8).toByte())) }
    fun writeFloat(v: Float) { val bits = java.lang.Float.floatToIntBits(v); writeLE32(bits) }
    fun writeFixedStr(s: String, len: Int) {
        val bytes = s.toByteArray(Charsets.ISO_8859_1)
        val padded = ByteArray(len)
        bytes.copyInto(padded, 0, 0, minOf(len, bytes.size))
        out.write(padded)
    }
    fun writeNullStr(s: String) { out.write(s.toByteArray(Charsets.ISO_8859_1)); out.write(0) }

    writeLE32(file.header.toInt())
    writeLE32(file.rows.size)
    writeLE32(file.defaultRecordLength.toInt())
    writeLE32(file.columns.size)

    for (col in file.columns) {
        writeFixedStr(col.name, 0x30)
        writeLE32(col.type.toInt())
        writeLE32(col.length)
    }

    for (row in file.rows) {
        val rowStart = out.size()
        writeLE16(0) // placeholder for row length
        for ((col, value) in file.columns.zip(row)) {
            when (col.type.toInt()) {
                1, 12, 0x10       -> out.write((value as? Long ?: 0L).toInt() and 0xFF)
                2                 -> writeLE16((value as? Long ?: 0L).toInt() and 0xFFFF)
                3, 11, 0x12, 0x1b -> writeLE32((value as? Long ?: 0L).toInt())
                5                 -> writeFloat((value as? Double ?: 0.0).toFloat())
                13, 0x15          -> writeLE16((value as? Long ?: 0L).toInt() and 0xFFFF)
                0x16              -> writeLE32((value as? Long ?: 0L).toInt())
                20                -> out.write((value as? Long ?: 0L).toInt() and 0xFF)
                9, 0x18           -> writeFixedStr(value as? String ?: "", col.length)
                0x1a              -> writeNullStr(value as? String ?: "")
                else              -> out.write((value as? Long ?: 0L).toInt() and 0xFF)
            }
        }
        // Back-fill row length
        val rowLen = out.size() - rowStart
        val buf = out.toByteArray()
        buf[rowStart]     = (rowLen and 0xFF).toByte()
        buf[rowStart + 1] = ((rowLen shr 8) and 0xFF).toByte()
        out.reset()
        out.write(buf)
    }

    val payload = out.toByteArray()
    shnCrypt(payload)  // encrypt == decrypt (symmetric)

    val result = ByteArrayOutputStream()
    result.write(file.cryptHeader)
    val totalLen = payload.size + 0x24
    writeLE32(totalLen).also { result.write(byteArrayOf(totalLen.toByte(), (totalLen shr 8).toByte(), (totalLen shr 16).toByte(), (totalLen shr 24).toByte())) }
    result.write(payload)
    return result.toByteArray()
}

// ─── ViewModel ───────────────────────────────────────────────────────────────

class SHNViewModel : ViewModel() {

    private val _state = MutableStateFlow(SHNState())
    val state: StateFlow<SHNState> = _state.asStateFlow()

    // mutable copy of rows for editing
    private val _editRows = mutableStateListOf<MutableList<Any>>()
    val editRows: List<MutableList<Any>> get() = _editRows

    fun loadFromStream(stream: InputStream, name: String) {
        viewModelScope.launch {
            _state.update { it.copy(loading = true, error = null) }
            runCatching {
                withContext(Dispatchers.IO) {
                    val bytes = stream.readBytes()
                    parseSHN(bytes)
                }
            }.onSuccess { file ->
                _editRows.clear()
                file.rows.forEach { row -> _editRows.add(row.toMutableList()) }
                _state.update { it.copy(loading = false, file = file, fileName = name, dirtyRows = emptySet(), error = null) }
            }.onFailure { e ->
                _state.update { it.copy(loading = false, error = e.message ?: "parse error") }
            }
        }
    }

    fun setCell(rowIdx: Int, colIdx: Int, raw: String) {
        val col = _state.value.file?.columns?.getOrNull(colIdx) ?: return
        val parsed: Any = when (col.type.toInt()) {
            1, 12, 0x10, 2, 3, 11, 0x12, 0x1b, 13, 0x15, 0x16, 20 ->
                raw.toLongOrNull() ?: _editRows[rowIdx][colIdx]
            5    -> raw.toDoubleOrNull() ?: _editRows[rowIdx][colIdx]
            else -> raw
        }
        _editRows[rowIdx][colIdx] = parsed
        _state.update { it.copy(dirtyRows = it.dirtyRows + rowIdx, saved = false) }
    }

    fun addRow() {
        val file = _state.value.file ?: return
        val emptyRow: MutableList<Any> = file.columns.map { col ->
            when (col.type.toInt()) {
                5    -> 0.0
                9, 0x18, 0x1a -> ""
                else -> 0L
            }
        }.toMutableList()
        _editRows.add(emptyRow)
        _state.update { it.copy(dirtyRows = it.dirtyRows + (_editRows.size - 1), saved = false) }
    }

    fun deleteRow(idx: Int) {
        _editRows.removeAt(idx)
        _state.update { it.copy(dirtyRows = emptySet(), saved = false) }
    }

    fun onSearch(q: String) = _state.update { it.copy(search = q) }
    fun onFilterCol(c: Int) = _state.update { it.copy(filterCol = c) }

    fun getFilteredIndices(): List<Int> {
        val s = _state.value
        val q = s.search.trim().lowercase()
        if (q.isEmpty()) return _editRows.indices.toList()
        return _editRows.indices.filter { i ->
            _editRows[i].getOrNull(s.filterCol)?.toString()?.lowercase()?.contains(q) == true
        }
    }

    fun saveTo(
        file:     SHNFile,
        fileName: String,
        onBytes:  (ByteArray, String) -> Unit,
    ) {
        viewModelScope.launch {
            _state.update { it.copy(saving = true, error = null) }
            runCatching {
                withContext(Dispatchers.IO) {
                    val updated = file.copy(rows = _editRows.map { it.toList() })
                    serializeSHN(updated)
                }
            }.onSuccess { bytes ->
                _state.update { it.copy(saving = false, dirtyRows = emptySet(), saved = true) }
                onBytes(bytes, fileName)
            }.onFailure { e ->
                _state.update { it.copy(saving = false, error = e.message ?: "save error") }
            }
        }
    }
}

// ─── Screen ──────────────────────────────────────────────────────────────────

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SHNScreen(
    onSaveToServer: ((ByteArray, String) -> Unit)? = null,
) {
    val vm: SHNViewModel = viewModel()
    val state by vm.state.collectAsState()
    val ctx = LocalContext.current

    // File picker
    val filePicker = rememberLauncherForActivityResult(
        ActivityResultContracts.GetContent()
    ) { uri: Uri? ->
        if (uri == null) return@rememberLauncherForActivityResult
        val name = uri.lastPathSegment?.substringAfterLast('/') ?: "unknown.shn"
        ctx.contentResolver.openInputStream(uri)?.use { stream ->
            vm.loadFromStream(stream, name)
        }
    }

    // Save to device
    val saveToDevice = rememberLauncherForActivityResult(
        ActivityResultContracts.CreateDocument("application/octet-stream")
    ) { uri: Uri? ->
        if (uri == null) return@rememberLauncherForActivityResult
        val file = state.file ?: return@rememberLauncherForActivityResult
        vm.saveTo(file, state.fileName) { bytes, _ ->
            ctx.contentResolver.openOutputStream(uri)?.use { it.write(bytes) }
        }
    }

    Scaffold(
        containerColor = IsyaNight,
        topBar = {
            IsyaTopBar(
                title    = { Text(if (state.fileName.isEmpty()) "SHN Editor" else state.fileName) },
                subtitle = state.file?.let { file ->
                    { Text("${file.rows.size} records · ${file.columns.size} columns") }
                },
                actions  = {
                    if (state.file != null) {
                        // Save to server
                        if (onSaveToServer != null) {
                            IconButton(onClick = {
                                vm.saveTo(state.file!!, state.fileName) { bytes, name ->
                                    onSaveToServer(bytes, name)
                                }
                            }, enabled = !state.saving) {
                                if (state.saving)
                                    CircularProgressIndicator(Modifier.size(18.dp), color = IsyaMagic, strokeWidth = 2.dp)
                                else
                                    Icon(Icons.Rounded.CloudUpload, "Save to server", tint = IsyaMagic)
                            }
                        }
                        // Save to device
                        IconButton(onClick = { saveToDevice.launch(state.fileName) }) {
                            Icon(Icons.Rounded.SaveAlt, "Save to device", tint = IsyaGold)
                        }
                        // Add row
                        IconButton(onClick = vm::addRow) {
                            Icon(Icons.Rounded.AddCircleOutline, "Add row", tint = IsyaSuccess)
                        }
                    }
                    // Open file
                    IconButton(onClick = { filePicker.launch("*/*") }) {
                        Icon(Icons.Rounded.FolderOpen, "Open SHN", tint = IsyaCream)
                    }
                }
            )
        }
    ) { padding ->
        Column(Modifier.fillMaxSize().padding(padding)) {

            // Error
            state.error?.let {
                IsyaErrorState(it)
            }

            // Saved banner
            if (state.saved) {
                Box(
                    Modifier.fillMaxWidth().background(IsyaSuccess.copy(alpha = 0.15f)).padding(8.dp),
                    contentAlignment = Alignment.Center,
                ) {
                    Text("Saved", color = IsyaSuccess, fontSize = 12.sp)
                }
            }

            when {
                state.loading -> {
                    Box(Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
                        CircularProgressIndicator(color = IsyaMagic)
                    }
                }

                state.file == null -> {
                    Box(Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
                        Column(horizontalAlignment = Alignment.CenterHorizontally, verticalArrangement = Arrangement.spacedBy(12.dp)) {
                            Icon(Icons.Rounded.TableChart, null, tint = IsyaMuted, modifier = Modifier.size(48.dp))
                            Text("Open a .shn file to edit", color = IsyaMuted, fontSize = 14.sp)
                            OutlinedButton(
                                onClick = { filePicker.launch("*/*") },
                                colors = ButtonDefaults.outlinedButtonColors(contentColor = IsyaMagic),
                                border = androidx.compose.foundation.BorderStroke(1.dp, IsyaMagic.copy(alpha = 0.5f)),
                            ) {
                                Text("Open file")
                            }
                        }
                    }
                }

                else -> {
                    val file = state.file!!
                    SHNTableView(
                        vm       = vm,
                        file     = file,
                        state    = state,
                    )
                }
            }
        }
    }
}

// ─── Table view ──────────────────────────────────────────────────────────────

@Composable
private fun SHNTableView(
    vm:    SHNViewModel,
    file:  SHNFile,
    state: SHNState,
) {
    val hScroll = rememberScrollState()

    Column(Modifier.fillMaxSize()) {
        // Search bar
        Row(
            Modifier.fillMaxWidth().padding(horizontal = 8.dp, vertical = 4.dp),
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.spacedBy(8.dp),
        ) {
            IsyaSearchBar(
                value         = state.search,
                onValueChange = vm::onSearch,
                placeholder   = "Search column…",
                modifier      = Modifier.weight(1f),
            )
            // Column filter dropdown
            if (file.columns.isNotEmpty()) {
                var expanded by remember { mutableStateOf(false) }
                Box {
                    OutlinedButton(
                        onClick = { expanded = true },
                        contentPadding = PaddingValues(horizontal = 8.dp, vertical = 4.dp),
                        colors = ButtonDefaults.outlinedButtonColors(contentColor = IsyaMuted),
                        border = androidx.compose.foundation.BorderStroke(1.dp, IsyaMuted.copy(alpha = 0.3f)),
                    ) {
                        Text(
                            file.columns.getOrNull(state.filterCol)?.name?.take(10) ?: "Col",
                            fontSize = 11.sp,
                            maxLines = 1,
                        )
                        Icon(Icons.Rounded.ArrowDropDown, null, Modifier.size(16.dp))
                    }
                    DropdownMenu(
                        expanded         = expanded,
                        onDismissRequest = { expanded = false },
                        modifier         = Modifier.background(IsyaNight),
                    ) {
                        file.columns.forEachIndexed { i, col ->
                            DropdownMenuItem(
                                text    = { Text(col.name, color = IsyaCream, fontSize = 12.sp, fontFamily = FontFamily.Monospace) },
                                onClick = { vm.onFilterCol(i); expanded = false },
                            )
                        }
                    }
                }
            }
        }

        // Column headers
        Row(
            Modifier
                .fillMaxWidth()
                .horizontalScroll(hScroll)
                .background(IsyaNight)
                .border(1.dp, IsyaMuted.copy(alpha = 0.2f)),
        ) {
            // Row # header
            SHNHeaderCell("#", 40.dp)
            file.columns.forEach { col ->
                SHNHeaderCell(col.name, colWidth(col))
            }
            // Delete column header
            SHNHeaderCell("", 40.dp)
        }

        // Rows
        val indices = vm.getFilteredIndices()
        LazyColumn(Modifier.fillMaxSize()) {
            itemsIndexed(indices) { _, rowIdx ->
                val isDirty = rowIdx in state.dirtyRows
                Row(
                    Modifier
                        .fillMaxWidth()
                        .horizontalScroll(hScroll)
                        .background(
                            if (isDirty) IsyaWarn.copy(alpha = 0.07f)
                            else if (rowIdx % 2 == 0) IsyaNight else IsyaNight.copy(alpha = 0.7f)
                        )
                        .border(
                            width = if (isDirty) 1.dp else 0.dp,
                            color = if (isDirty) IsyaWarn.copy(alpha = 0.3f) else IsyaNight,
                        ),
                    verticalAlignment = Alignment.CenterVertically,
                ) {
                    // Row index
                    Box(Modifier.width(40.dp).padding(2.dp), contentAlignment = Alignment.Center) {
                        Text("$rowIdx", color = IsyaMuted, fontSize = 10.sp, fontFamily = FontFamily.Monospace)
                    }
                    // Cells
                    file.columns.forEachIndexed { colIdx, col ->
                        SHNCell(
                            value    = vm.editRows.getOrNull(rowIdx)?.getOrNull(colIdx)?.toString() ?: "",
                            width    = colWidth(col),
                            isString = col.type.toInt() in listOf(9, 0x18, 0x1a),
                            onChange = { vm.setCell(rowIdx, colIdx, it) },
                        )
                    }
                    // Delete row
                    IconButton(
                        onClick  = { vm.deleteRow(rowIdx) },
                        modifier = Modifier.size(40.dp),
                    ) {
                        Icon(Icons.Rounded.DeleteOutline, "Delete row", tint = IsyaError.copy(alpha = 0.6f), modifier = Modifier.size(16.dp))
                    }
                }
            }

            item {
                Spacer(Modifier.height(80.dp))
            }
        }
    }
}

@Composable
private fun SHNHeaderCell(label: String, width: androidx.compose.ui.unit.Dp) {
    Box(
        Modifier
            .width(width)
            .padding(horizontal = 4.dp, vertical = 6.dp),
        contentAlignment = Alignment.CenterStart,
    ) {
        Text(
            label,
            color      = IsyaGold,
            fontSize   = 10.sp,
            fontWeight = FontWeight.Bold,
            fontFamily = FontFamily.Monospace,
            maxLines   = 1,
            overflow   = TextOverflow.Ellipsis,
        )
    }
}

@Composable
private fun SHNCell(
    value:    String,
    width:    androidx.compose.ui.unit.Dp,
    isString: Boolean,
    onChange: (String) -> Unit,
) {
    var text by remember(value) { mutableStateOf(value) }

    BasicTextField(
        value         = text,
        onValueChange = { text = it; onChange(it) },
        singleLine    = !isString,
        textStyle     = LocalTextStyle.current.copy(
            color      = if (isString) IsyaMagic.copy(alpha = 0.9f) else IsyaCream,
            fontSize   = 11.sp,
            fontFamily = FontFamily.Monospace,
        ),
        cursorBrush   = SolidColor(IsyaMagic),
        modifier      = Modifier
            .width(width)
            .padding(horizontal = 4.dp, vertical = 4.dp),
    )
}

private fun colWidth(col: SHNColumn): androidx.compose.ui.unit.Dp = when (col.type.toInt()) {
    9, 0x18, 0x1a -> maxOf(120, col.length * 7).coerceAtMost(240).dp
    5             -> 80.dp
    3, 11, 0x12   -> 80.dp
    else          -> 64.dp
}
