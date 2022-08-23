FreeCam = ScriptComponent
{
	speed = 20;
	sens = 20;
	maxCamSpin = 90;
}

-- OnUpdate called every frame.
function FreeCam: OnUpdate(dt)
	local angels = self.entity.transform:GetAngels ();
	angels.y = angels.y + self.sens * dt * Input:GetMouseXDelta ();
	angels.x = angels.x + self.sens * dt * -Input:GetMouseYDelta ();
	angels.x = Mathf.Clamp (angels.x, -self.maxCamSpin, self.maxCamSpin);
	self.entity.transform:SetAngels (angels);
	
	local pos = self.entity.transform:GetPosition();
	pos = pos + self.entity.transform:Forward () * Vector3.Vec3F (self.speed * dt * Input:GetAxis ("Vertical"));
	pos = pos + self.entity.transform:Right () * Vector3.Vec3F (self.speed * dt * Input:GetAxis ("Horizontal"));
	self.entity.transform:SetPosition(pos);
	--self.entity.transform:LookAt(Vec3F(0.0));
end


