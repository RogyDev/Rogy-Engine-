-- Script Component
---------------------------------------------
null = nil;
Scene = nil;
Input = nil;
Physics = nil;
Debug = nil;
Audio = nil;

function Prepare_ScriptSide(scene, input, phy, debuger, audio)
	Scene = scene;
	Input = input;
	Physics = phy;
	Debug = debuger;
  Audio = audio;
end

function ScriptComponent()
	return Class:extend({})
end