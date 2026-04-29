/*══════════════════════════════════════════════════════════════════════════════
 * AppShell.jsx — left-rail navigation + auth gate for the dev panel.
 *══════════════════════════════════════════════════════════════════════════════*/
import { NavLink, Outlet, useNavigate } from "react-router-dom";
import {
  Activity,
  MessageSquare,
  Brain,
  Heart,
  Target,
  Users,
  Cpu,
  Cog,
  Gamepad2,
  ScrollText,
  LogOut,
  Sparkles,
} from "lucide-react";
import { auth } from "@/lib/api";

const NAV = [
  { to: "/",           label: "Dashboard",  icon: Activity    },
  { to: "/chat",       label: "Chat",       icon: MessageSquare },
  { to: "/memory",     label: "Memory",     icon: Brain       },
  { to: "/emotion",    label: "Emotion",    icon: Heart       },
  { to: "/goals",      label: "Goals",      icon: Target      },
  { to: "/bonding",    label: "Bonding",    icon: Users       },
  { to: "/llm",        label: "LLM",        icon: Sparkles    },
  { to: "/diag",       label: "Diagnostics",icon: Cpu         },
  { to: "/fiesta",     label: "Fiesta",     icon: Gamepad2    },
  { to: "/logs",       label: "Logs",       icon: ScrollText  },
  { to: "/config",     label: "Config",     icon: Cog         },
];

export default function AppShell() {
  const nav = useNavigate();
  const onLogout = () => {
    auth.logout();
    nav("/login");
  };

  return (
    <div className="flex h-screen bg-zinc-950 text-zinc-100">
      <aside
        className="w-56 shrink-0 border-r border-zinc-800 bg-zinc-900/60 backdrop-blur flex flex-col"
        data-testid="app-sidebar"
      >
        <div className="px-5 py-5 border-b border-zinc-800">
          <div className="text-xs uppercase tracking-[0.2em] text-zinc-500">
            Elle-Ann
          </div>
          <div className="text-sm font-semibold text-zinc-100 mt-0.5">
            Dev Panel
          </div>
          <div className="text-xs text-zinc-500 mt-0.5" data-testid="user-username">
            {auth.username() || "—"}
          </div>
        </div>
        <nav className="flex-1 px-2 py-3 space-y-0.5 overflow-y-auto">
          {NAV.map((n) => (
            <NavLink
              key={n.to}
              to={n.to}
              end={n.to === "/"}
              data-testid={`nav-${n.label.toLowerCase()}`}
              className={({ isActive }) =>
                `flex items-center gap-2.5 rounded-md px-3 py-1.5 text-sm transition-colors ${
                  isActive
                    ? "bg-emerald-500/10 text-emerald-300"
                    : "text-zinc-400 hover:text-zinc-100 hover:bg-zinc-800/60"
                }`
              }
            >
              <n.icon className="h-4 w-4 shrink-0" />
              <span>{n.label}</span>
            </NavLink>
          ))}
        </nav>
        <div className="px-2 py-3 border-t border-zinc-800">
          <button
            onClick={onLogout}
            data-testid="logout-button"
            className="w-full flex items-center gap-2.5 rounded-md px-3 py-1.5 text-sm text-zinc-400 hover:text-rose-300 hover:bg-rose-500/10 transition-colors"
          >
            <LogOut className="h-4 w-4" />
            Sign out
          </button>
        </div>
      </aside>

      <main className="flex-1 overflow-y-auto" data-testid="page-main">
        <Outlet />
      </main>
    </div>
  );
}
