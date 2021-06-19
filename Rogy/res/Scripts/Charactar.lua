Charactar = ScriptComponent()  

-- class constructor, define instance variables
function Charactar: init()
  self.Speed = 2000;
  self.LookSpeed = 30;
  self.maxCamSpin = 90;
  self.JumpHeight = 7.0;
  
  self._onGround = false;
  self.cam = nil;
  self.rigidbody = nil;
end

-- OnStart called when the entity starts
function Charactar: OnStart()
  self.cam = self.entity:GetChild ("Cam");
  self.rigidbody  = self.entity:GetRigidBody();
  Input:SetCursor(false);
end

function Charactar:PrintName()
  Debug:Log("Charactar : " .. self.entity.name)
end

function Charactar:DoGroundTest()
  self._onGround = Physics:RayTest(self.entity.transform:GetPosition(), Vector3(0.0, -1.0, 0.0), 2.0);
end

function Charactar:Jump()
	self.rigidbody:AddCentralForce (Vector3(0.0, Mathf.Sqrt(self.JumpHeight * -2 * Physics:GetGravity().y), 0.0));
end

-- OnUpdate called every frame.
function Charactar: OnUpdate(dt)
  
  self:DoGroundTest();
  
  -- Rotate charactar
	local angleY = self.LookSpeed * dt * Input:GetMouseXDelta ();
	self.entity:RotateY(angleY);
  
  -- Rotate camera
	local Camangels = self.cam.transform:GetAngels ();
	Camangels.x = Camangels.x + self.LookSpeed * dt * -Input:GetMouseYDelta ();
	Camangels.x = Mathf.Clamp (Camangels.x, -self.maxCamSpin, self.maxCamSpin);
  self.cam.transform:SetAngels (Camangels);
  
  --if not self._onGround then return end
  -- Move charactar
	local velo = Vector3(0, 0, 0);
	--velo = velo + self.entity.transform:Forward () * Vector3.Vec3F (self.Speed * dt);
	velo = velo + self.entity.transform:Forward () * Vector3.Vec3F (self.Speed * dt * Input:GetAxis ("Vertical"));
	velo = velo + self.entity.transform:Right () * Vector3.Vec3F (self.Speed * dt * Input:GetAxis ("Horizontal"));
  self.rigidbody:Move (velo);
	
	if(self._onGround and Input:GetKeyDown(RKey.Space)) then
    	self:Jump();
	end
end










