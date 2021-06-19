-- Script Component
---------------------------------------------
null = nil;
Scene = nil;
Input = nil;
Physics = nil;
Debug = nil;
Audio = nil;
PlayerPrefs = nil;

Asset = {
	Prefab = "Prefab"
}

function Prepare_ScriptSide(scene, input, phy, debuger, audio)
	Scene = scene;
	Input = input;
	Physics = phy;
	Debug = debuger;
    Audio = audio;
end

function Prepare_ScriptSide2(pprefs)
	PlayerPrefs = pprefs;
end

function ScriptComponent()
	return Class:extend({})
end

-- Editor Utilities ------
function AssetType(bb, var_name)
	return bb[var_name]:GetType();
end

function GetAssetPath(bb, var_name)
	return bb[var_name]:Get();
end

function SetAssetPath(bb, var_name, new_path)
	return bb[var_name]:Set(new_path);
end

function SetAssetPath(bb, var_name, new_path)
	return bb[var_name]:Set(new_path);
end

function GetVec3XYZ(bb, var_name, idx)
	if(idx == 0) then
		return bb[var_name].x;
	elseif(idx == 1) then
		return bb[var_name].y;
	end
	return bb[var_name].z;
end

function SetVec3XYZ(bb, var_name, idx, val)
	if(idx == 0) then
		bb[var_name].x = val;
	elseif(idx == 1) then
		bb[var_name].y = val;
	end
	bb[var_name].z = val;
end