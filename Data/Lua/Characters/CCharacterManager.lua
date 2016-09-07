dofile("Data\\Lua\\Player\\CPlayer.lua")
dofile("Data\\Lua\\Enemies\\CEnemy.lua")
dofile("Data\\Lua\\Enemies\\FogEnemy\\FogEnemy.lua")
dofile("Data\\Lua\\Enemies\\AutomatonEnemy\\AutomatonEnemy.lua")
dofile("Data\\Lua\\Enemies\\CagedEnemy\\CagedAutomatonEnemy.lua")
dofile("Data\\Lua\\Enemies\\TurretEnemy\\TurretEnemy.lua")

class 'CCharacterManager'
	function CCharacterManager:__init()
		self.m_Enemics={}
		self.m_Player={}
	end
		
	function CCharacterManager:LoadXML(Filename)
		local doc = XMLDocument()
		local xmlError = doc:load_file(Filename)
		local UABEngine = CUABEngine.get_instance()
		UABEngine:get_game_play_manager():destroy()
		if xmlError == 0 then
			local l_Element = doc:first_child_element("characters"):first_child()
			while l_Element ~= nil do
				local l_ElemName=l_Element:get_name()
				if l_ElemName=="player" then
					g_Player = CPlayer(l_Element)
					UABEngine:get_game_play_manager():add_component(g_Player)
					table.insert(self.m_Player, g_Player)
				elseif l_ElemName == "enemy" then
					local l_Type = l_Element:get_psz_property("type", "")
					
					if l_Type == "Automaton" then
						local l_Enemy = CAutomatonEnemy(l_Element)
						UABEngine:get_game_play_manager():add_component(l_Enemy)
						table.insert(self.m_Enemics, l_Enemy)			
					elseif l_Type == "FogAutomaton" then
						local l_Enemy = CFogEnemy(l_Element)
						UABEngine:get_game_play_manager():add_component(l_Enemy)
						table.insert(self.m_Enemics, l_Enemy)
					elseif l_Type == "CagedAutomaton" then
						local l_Enemy = CCagedAutomatonEnemy(l_Element)
						UABEngine:get_game_play_manager():add_component(l_Enemy)
						table.insert(self.m_Enemics, l_Enemy)	
					elseif l_Type == "Turret" then
						local l_Enemy = CTurretEnemy(l_Element)
						UABEngine:get_game_play_manager():add_component(l_Enemy)
						table.insert(self.m_Enemics, l_Enemy)
					elseif l_Type == "Whisperer" then
						utils_log("other")
					end					
				end	
				l_Element = l_Element:get_next()
			end
		else
			utils_log("File '"..Filename.."'not correctly loaded")
		end
	end
--end