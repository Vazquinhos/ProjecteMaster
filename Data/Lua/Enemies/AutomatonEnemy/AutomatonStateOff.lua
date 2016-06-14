function OffFirstAutomaton(args)
	utils_log("OffFirst")
	local l_Owner = args["owner"]
	l_Owner:clear_cycle(1,0.5)
	l_Owner:clear_cycle(2,0.5)
	l_Owner:blend_cycle(0,1.0,0.5)
end

function OffUpdateAutomaton(args, _ElapsedTime)
	local l_Enemy = args["self"]
	local l_Distance = g_Player.m_RenderableObject:get_position():distance(l_Enemy.m_RenderableObject:get_position())
	
	if l_Distance < l_Enemy.m_distance_to_activate or l_Enemy.m_Patrol then
		l_Enemy.m_State = "idle"
	end
end

function OffEndAutomaton(args)
end

function OffToIdleConditionAutomaton(args)	
	local l_Enemy = args["self"]
	return l_Enemy.m_State == "idle"
end