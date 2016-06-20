class 'CEnemy' (CLUAComponent)
	function CEnemy:__init(_TreeNode)
		local UABEngine = CUABEngine.get_instance()
		self.m_Name = _TreeNode:get_psz_property("name", "", false)
		self.m_LayerName = _TreeNode:get_psz_property("layer", "", false)
		self.m_RenderableObjectName = _TreeNode:get_psz_property("renderable_object", "", false)
		self.m_RenderableObject = UABEngine:get_layer_manager():get_resource(self.m_LayerName):get_resource(self.m_RenderableObjectName)
		CLUAComponent.__init(self,self.m_Name, self.m_RenderableObject)		
		self.m_PhysXManager = CUABEngine.get_instance():get_physX_manager()
		self.m_PathFindig = CAStar()
		self.m_Velocity = Vect3f(0,0,0)
		self.m_Gravity = -9.81
		self.m_WalkSpeed = 0.5
		self.m_RunSpeed = 2.5
		self.m_AngularWalkSpeed = 1000.0
		self.m_AngularRunSpeed = 250.0
		self.m_TimerRotation = 0.0
		self.m_Timer = 0
		self.m_DefaultPosition = Vect3f(self.m_RenderableObject:get_position().x, self.m_RenderableObject:get_position().y, self.m_RenderableObject:get_position().z)
		self.m_Patrol = _TreeNode:get_bool_property("patrol", false, false)
		self.m_State = "off"
		
		-- distance, time, sanity amount
		self.m_LoseSanity = {}
		for i = 0, _TreeNode:get_num_children()-1 do
			local l_Param = _TreeNode:get_child(i)
			table.insert(self.m_LoseSanity, Vect3f(l_Param:get_float_property("distance",0.0,false), l_Param:get_float_property("time",0.0,false), l_Param:get_float_property("sanity_amount",0.0,false)))
		end
		
		-- TODO: get group numbers somehow
		-- at the moment bit 0: plane, bit 1: objects, bit 2: triggers, bit 3: player
		self.m_PhysXGroups = 2 + 8 -- objects and player
		self.m_MaxDistance = 25.0
		self.m_MaxAngle = 0.25 * math.pi
		self.m_HeadOffset = Vect3f(0.0, 1.7, 0.0)
		self.m_BlockingObjectName = nil
		self.m_StandardAlertTime = 3.0
		
		self.m_distance_to_activate = 10.0
		self.m_off = true
		self.m_time_to_teleport = 1.0
		self.m_teleport_distance = 5.0
		self.m_teleport_timer = 0
		self.m_time_sin_mirar = 1.5
		self.m_timer_to_stop = 0
		self.m_alert_timer = 0
		self.m_distance_to_kill = 1.0
		
		self.m_StateMachine = StateMachine.create()
		if(not UABEngine:get_lua_reloaded())then
			self.m_PhysXManager:create_character_controller(self.m_Name, 1.2, 0.3, 0.5, self.m_RenderableObject:get_position(),"controllerMaterial", "Enemy")
		end
	end
	
	function CEnemy:Update(_ElapsedTime)
		utils_log("CEnemy:Update")
	end
	
	function CEnemy:PlayerVisible(_Owner)
		local l_OwnerHeadPos = _Owner:get_position() + self.m_HeadOffset
		local l_PlayerPos = self.m_PhysXManager:get_character_controler_pos("player")
		
		-- not visible if too far
		local l_Dist = l_PlayerPos:distance(l_OwnerHeadPos)
		if l_Dist > self.m_MaxDistance then
			return false
		end

		-- not visible if out of angle
		local l_PlayerDirection = l_PlayerPos - l_OwnerHeadPos
		l_PlayerDirection:normalize(1.0)
		local l_Forward = _Owner:get_rotation():get_forward_vector()
		local l_Dot = l_Forward * l_PlayerDirection
		if l_Dot < math.cos(self.m_MaxAngle) then
		  return false
		end

		-- not visible if behind an obstacle
		-- TODO: some raycasts from enemy's head to different parts of player
		local l_RaycastData = RaycastData()
		local l_Hit = self.m_PhysXManager:raycast(
			l_OwnerHeadPos, l_PlayerPos,
			self.m_PhysXGroups, l_RaycastData
		)
		
		if l_Hit and l_RaycastData.actor_name ~= "player" then
		  self.m_BlockingObjectName = l_RaycastData.actor_name
		  return false
		end

		-- otherwise visible
		self.m_BlockingObjectName = nil
		return true
	end
	
	function CEnemy:LoseSanity(_Distance)
		for i=1, table.maxn(self.m_LoseSanity)-1 do
			if _Distance <= self.m_LoseSanity[i].x and _Distance > self.m_LoseSanity[i+1].x and self.m_Timer >= self.m_LoseSanity[i].y then
				g_Player:ModifySanity(self.m_LoseSanity[i].z)
				self.m_Timer = 0
				break
			end
		end
	end
--end