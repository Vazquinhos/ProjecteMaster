function FogOffFirst(args)
	utils_log("FogOffFirstTime")
end

function FogOffUpdate(args, _ElapsedTime)
end

function FogOffEnd(args)
	utils_log("FogOffEnd")
end

function FogOffToTeleportCondition()
	return m_CharacterManager.m_Enemics[1].m_off == false
end