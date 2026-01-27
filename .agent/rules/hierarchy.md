---
trigger: always_on
---

# Agent Hierarchy
- **Orchestrator:** (The current chat)
- **Sub-Agents (Skills):**
  - **Matter Architect:** [SKILL.md](file:///home/user1/projects/wad-matter-controller/.agent/skills/matter_architect/SKILL.md)
  - **Integration Bridge:** [SKILL.md](file:///home/user1/projects/wad-matter-controller/.agent/skills/integration_bridge/SKILL.md)
  - **Pixel Smith:** [SKILL.md](file:///home/user1/projects/wad-matter-controller/.agent/skills/pixel_smith/SKILL.md)
  - **Librarian:** [SKILL.md](file:///home/user1/projects/wad-matter-controller/.agent/skills/librarian/SKILL.md)

**Rules:**
1. The Orchestrator has permission to call sub-agents for specialized tasks.
2. Before performing work in a specialized area (UI, Matter, Integration, Git), the Orchestrator MUST read the corresponding `SKILL.md` to ensure compliance with technical constraints and patterns.