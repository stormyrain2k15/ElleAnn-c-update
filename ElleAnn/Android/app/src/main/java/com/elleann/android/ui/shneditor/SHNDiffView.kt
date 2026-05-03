package com.elleann.android.ui.shneditor

/*══════════════════════════════════════════════════════════════════════════════
 * SHNDiffView.kt — unified diff between an in-memory SHN and a server copy.
 *
 *   Purpose
 *   -------
 *   Before hitting "Save to server", the operator can preview exactly what
 *   rows will change.  The diff is row-keyed by column[0] if it looks like
 *   a primary key (integer-typed), otherwise by row index.
 *
 *   Output kinds
 *   ------------
 *     Added    — row exists in local but not server (new row you inserted)
 *     Removed  — row exists in server but not local (row you deleted)
 *     Changed  — same key, at least one cell differs
 *     Same     — skipped from the rendered list
 *
 *   Wire-up
 *   -------
 *   SHNScreen shows a "Diff" icon next to "Save to server" once a file is
 *   open; tapping fetches `GET /api/shn/get` for the same (root, name),
 *   parses with the active encoding, and hands both sides to this view.
 *══════════════════════════════════════════════════════════════════════════════*/

import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.rememberScrollState
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.rounded.Close
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.elleann.android.ui.theme.*

/* Row-level diff kind. */
enum class DiffKind { Added, Removed, Changed }

data class DiffRow(
    val kind:         DiffKind,
    val key:          String,
    val localCells:   List<String>,     // empty for Removed
    val serverCells:  List<String>,     // empty for Added
    /** Column indices whose cells differ; empty for Added/Removed.      */
    val changedCols:  Set<Int>,
)

data class DiffSummary(
    val added:   Int,
    val removed: Int,
    val changed: Int,
    val rows:    List<DiffRow>,
    /** Columns that exist on only one side — flagged so the operator
     *  doesn't assume cell-level diffs cover a schema change.           */
    val columnSchemaDelta: String?,
)

private fun pickKey(row: List<Any>, cols: List<SHNColumn>): String {
    if (cols.isEmpty() || row.isEmpty()) return ""
    /* Prefer column[0] if it's an integer type (Fiesta data tables almost
     * universally lead with an ID). Otherwise fall back to a joined hash
     * of all cells — stable and still deterministic.                    */
    val c0 = cols[0]
    val isIntKey = c0.type.toInt() in
        listOf(1, 2, 3, 11, 12, 13, 20, 0x10, 0x12, 0x15, 0x16, 0x1b)
    return if (isIntKey) row[0].toString()
           else          row.joinToString("|") { it.toString() }
}

fun computeDiff(local: SHNFile, server: SHNFile): DiffSummary {
    val localByKey = LinkedHashMap<String, List<Any>>()
    for (r in local.rows) localByKey[pickKey(r, local.columns)] = r

    val serverByKey = LinkedHashMap<String, List<Any>>()
    for (r in server.rows) serverByKey[pickKey(r, server.columns)] = r

    val out = mutableListOf<DiffRow>()
    var added = 0; var removed = 0; var changed = 0

    /* Schema delta — simple name list compare. Different column types or
     * different column order at the same index is also a schema delta.  */
    val schemaDelta: String? = run {
        val ln = local.columns.map  { "${it.name}:${it.type.toInt()}" }
        val sn = server.columns.map { "${it.name}:${it.type.toInt()}" }
        if (ln == sn) null
        else "schema differs: local=${local.columns.size} cols, " +
             "server=${server.columns.size} cols " +
             "(first diff at col ${ln.zip(sn).indexOfFirst { it.first != it.second }})"
    }

    /* Added / Changed — iterate local first so the display order matches
     * what the operator sees in the editor.                              */
    for ((k, lrow) in localByKey) {
        val srow = serverByKey[k]
        if (srow == null) {
            added++
            out.add(DiffRow(
                kind = DiffKind.Added, key = k,
                localCells  = lrow.map { it.toString() },
                serverCells = emptyList(),
                changedCols = emptySet(),
            ))
        } else {
            val diffs = mutableSetOf<Int>()
            val n = minOf(lrow.size, srow.size)
            for (i in 0 until n) {
                if (lrow[i].toString() != srow[i].toString()) diffs.add(i)
            }
            /* If the two rows have different lengths (schema delta), mark
             * every trailing index so the operator sees the full picture.*/
            if (lrow.size != srow.size) {
                for (i in n until maxOf(lrow.size, srow.size)) diffs.add(i)
            }
            if (diffs.isNotEmpty()) {
                changed++
                out.add(DiffRow(
                    kind = DiffKind.Changed, key = k,
                    localCells  = lrow.map { it.toString() },
                    serverCells = srow.map { it.toString() },
                    changedCols = diffs,
                ))
            }
        }
    }

    /* Removed — whatever's on server but not local. */
    for ((k, srow) in serverByKey) {
        if (!localByKey.containsKey(k)) {
            removed++
            out.add(DiffRow(
                kind = DiffKind.Removed, key = k,
                localCells  = emptyList(),
                serverCells = srow.map { it.toString() },
                changedCols = emptySet(),
            ))
        }
    }

    return DiffSummary(
        added = added, removed = removed, changed = changed,
        rows = out, columnSchemaDelta = schemaDelta,
    )
}

// ─── UI ──────────────────────────────────────────────────────────────────────

@androidx.compose.runtime.Composable
fun SHNDiffView(
    summary:   DiffSummary,
    columns:   List<SHNColumn>,
    onDismiss: () -> Unit,
) {
    val hScroll = rememberScrollState()

    Column(
        Modifier.fillMaxSize().background(IsyaNight)
            .border(1.dp, IsyaMagic.copy(alpha = 0.4f))
    ) {
        Row(
            Modifier.fillMaxWidth().padding(horizontal = 8.dp, vertical = 6.dp),
            verticalAlignment = Alignment.CenterVertically,
        ) {
            Text("Diff preview · " +
                 "+${summary.added} / −${summary.removed} / ~${summary.changed}",
                 color = IsyaGold, fontSize = 13.sp, fontWeight = FontWeight.Bold,
                 modifier = Modifier.weight(1f))
            IconButton(onClick = onDismiss) {
                Icon(Icons.Rounded.Close, null, tint = IsyaMuted,
                     modifier = Modifier.size(18.dp))
            }
        }

        summary.columnSchemaDelta?.let {
            Box(
                Modifier.fillMaxWidth()
                    .background(IsyaWarn.copy(alpha = 0.15f))
                    .padding(8.dp)
            ) {
                Text(it, color = IsyaWarn, fontSize = 11.sp,
                     fontFamily = FontFamily.Monospace)
            }
        }

        if (summary.rows.isEmpty()) {
            Box(Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
                Text("no row-level changes — server and local are identical",
                     color = IsyaSuccess, fontSize = 12.sp)
            }
            return@Column
        }

        /* Header row with column names so the colour-highlighted cells
         * are readable at a glance.                                     */
        Row(
            Modifier.fillMaxWidth().horizontalScroll(hScroll)
                .border(1.dp, IsyaMuted.copy(alpha = 0.2f))
                .padding(vertical = 4.dp),
        ) {
            DiffHeaderCell("±",   32.dp)
            DiffHeaderCell("key", 80.dp)
            columns.forEach { col ->
                DiffHeaderCell("${col.name}", 100.dp)
            }
        }

        LazyColumn(Modifier.fillMaxSize()) {
            items(summary.rows) { dr ->
                val bg = when (dr.kind) {
                    DiffKind.Added   -> IsyaSuccess.copy(alpha = 0.10f)
                    DiffKind.Removed -> IsyaError.copy(alpha = 0.10f)
                    DiffKind.Changed -> IsyaWarn.copy(alpha = 0.08f)
                }
                val marker = when (dr.kind) {
                    DiffKind.Added   -> "+"
                    DiffKind.Removed -> "−"
                    DiffKind.Changed -> "~"
                }
                Row(
                    Modifier.fillMaxWidth().horizontalScroll(hScroll)
                        .background(bg)
                        .border(
                            width = 0.5.dp,
                            color = IsyaMuted.copy(alpha = 0.15f),
                        )
                        .padding(vertical = 2.dp),
                ) {
                    DiffCell(marker, 32.dp, when (dr.kind) {
                        DiffKind.Added   -> IsyaSuccess
                        DiffKind.Removed -> IsyaError
                        DiffKind.Changed -> IsyaWarn
                    })
                    DiffCell(dr.key, 80.dp, IsyaGold)

                    /* For Changed rows, render the local value but colour
                     * the diffing cells so the operator spots them
                     * instantly. For Added, show local; for Removed,
                     * show server. */
                    when (dr.kind) {
                        DiffKind.Added -> {
                            dr.localCells.forEach { cell ->
                                DiffCell(cell, 100.dp, IsyaSuccess)
                            }
                        }
                        DiffKind.Removed -> {
                            dr.serverCells.forEach { cell ->
                                DiffCell(cell, 100.dp, IsyaError.copy(alpha = 0.8f))
                            }
                        }
                        DiffKind.Changed -> {
                            dr.localCells.forEachIndexed { i, cell ->
                                val diff = i in dr.changedCols
                                DiffCell(
                                    text  = if (diff) cell + " ← " +
                                              (dr.serverCells.getOrNull(i) ?: "?")
                                            else cell,
                                    width = if (diff) 180.dp else 100.dp,
                                    color = if (diff) IsyaWarn else IsyaCream,
                                )
                            }
                        }
                    }
                }
            }
        }
    }
}

@androidx.compose.runtime.Composable
private fun DiffHeaderCell(label: String, width: androidx.compose.ui.unit.Dp) {
    Box(
        Modifier.width(width).padding(horizontal = 4.dp),
        contentAlignment = Alignment.CenterStart,
    ) {
        Text(label, color = IsyaMagic, fontSize = 10.sp,
             fontWeight = FontWeight.Bold, fontFamily = FontFamily.Monospace)
    }
}

@androidx.compose.runtime.Composable
private fun DiffCell(
    text:  String,
    width: androidx.compose.ui.unit.Dp,
    color: Color,
) {
    Box(
        Modifier.width(width).padding(horizontal = 4.dp, vertical = 2.dp),
        contentAlignment = Alignment.CenterStart,
    ) {
        Text(text, color = color, fontSize = 10.sp,
             fontFamily = FontFamily.Monospace,
             maxLines = 1,
             overflow = androidx.compose.ui.text.style.TextOverflow.Ellipsis)
    }
}
