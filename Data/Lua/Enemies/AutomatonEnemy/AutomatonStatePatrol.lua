function PatrolFirstAutomaton(args)
	utils_log("PatrolFirst")
	local l_Owner = args["owner"]
	l_Owner:clear_cycle(0,0.5)
	l_Owner:clear_cycle(2,0.5)
	l_Owner:blend_cycle(1,1.0,0.5)
	
	local l_Enemy = args["self"]
	l_Enemy.m_PathFindig:load_map("Data\\level_"..g_Engine:get_level_loaded().."\\pathfinding.xml")
	l_Enemy.m_TimerRotation = 0.0
end

function PatrolUpdateAutomaton(args, _ElapsedTime)
	local l_Owner = args["owner"]
	local l_Enemy = args["self"]
	
	if l_Enemy:PlayerVisible(l_Owner) then
		l_Enemy.m_DefaultPosition = Vect3f(l_Owner:get_position().x, l_Owner:get_position().y, l_Owner:get_position().z)
		l_Enemy.m_TimerRotation = 0.0
		l_Enemy.m_State = "chase"
	else
		local l_NodePoint = l_Enemy.m_PathFindig:get_actual_patrol_point("path1")
		local l_PointPos = l_NodePoint.node.position
		local l_Distance = l_Enemy.m_RenderableObject:get_position():distance(l_PointPos)	
		
		if l_Distance <= 2.0 and l_NodePoint.wait == false then
			l_Enemy.m_PathFindig:increment_actual_patrol_point("path1")
			l_Enemy.m_TimerRotation = 0.0
		elseif l_Distance >= 0.0 and l_Distance <= 0.5 and l_NodePoint.wait then
			utils_log("Estamos en ALERTA!!!")
			l_Enemy.m_State = "alert"
		end
		
		l_Enemy.m_TimerRotation = l_Enemy.m_TimerRotation + _ElapsedTime
		local l_PercentRotation = l_Enemy.m_TimerRotation / l_Enemy.m_AngularWalkSpeed
		
		if l_PercentRotation > 1.0 then
			l_PercentRotation = 1.0
			l_Enemy.m_TimerRotation = 0.0
		end 
		
		l_Enemy:EnemyWalk(l_PointPos, l_Enemy.m_WalkSpeed, l_PercentRotation, _ElapsedTime)
	end
end

function PatrolEndAutomaton(args)
end

function PatrolToChaseConditionAutomaton(args)
	local l_Enemy = args["self"]
	return l_Enemy.m_State == "chase"
end

function PatrolToAlertConditionAutomaton(args)
	local l_Enemy = args["self"]
	return l_Enemy.m_State == "alert"
end