-- Test Class
----------------------------------------------
HeadBobbing = ScriptComponent() 

function HeadBobbing: init()
  self.bobMultiplier = 10.0;
  self.bobbingSpeed = 8.0;
  self.bobbingAmount = 0.02;
  self.moveAmount = 8.0;
  
  self._initPos = nil;
  self._timer = 0.0;
  self._pc = nil;
  self._fallOnce = false;
end

function HeadBobbing: OnStart()
  self._pc = GetScript("Charactar", self.entity.parent);
  self._initPos = self.entity.transform:GetPosition();
end

function HeadBobbing: OnUpdate(dt)

  
  if (self._pc._onGround and not self._fallOnce) then
    self._fallOnce = true;
    local amount = self.bobbingAmount * 4;
    self.entity.transform:SetPosition(self.entity.transform:GetPosition() + Vector3(0.0, -amount, 0.0));
  elseif (not self._pc._onGround and self._fallOnce) then
    self._fallOnce = false;
     local amount = self.bobbingAmount * 4;
    self.entity.transform:SetPosition(self.entity.transform:GetPosition() + Vector3(0.0, amount, 0.0));
  end
  
  if (not self._pc._onGround) then
    self.entity.transform:SetPosition(Vector3.Lerp(self.entity.transform:GetPosition(), self._initPos, dt * self.bobMultiplier));
    return;
  end
  
  local horizontal = Input:GetAxis("Horizontal");
  local vertical = Input:GetAxis("Vertical");
  
  local aHorz = Mathf.Abs(horizontal);
  local aVert = Mathf.Abs(vertical);

  local xMovement = 0.0;
  local yMovement = 0.0;

  local calcPosition = self.entity.transform:GetPosition();
  
  if (aHorz == 0 and aVert == 0) then
    timer = 0.0;
  else 
    xMovement = Mathf.Sin(self._timer);
    yMovement =-Mathf.Abs(Mathf.Abs(xMovement) - 1);
    
     self._timer =  self._timer + (self.bobbingSpeed * dt);
    
    if (timer > Mathf.PI() * 2) then
      self._timer = self._timer - (Mathf.PI() * 2);
    end
  end

  local totalMovement = Mathf.Clamp(aVert + aHorz, 0, 1) * 1.5;
  
  if (xMovement ~= 0) then
    xMovement = xMovement * totalMovement;
    calcPosition.x = self._initPos.x + xMovement *  self.bobbingAmount;
  else 
    calcPosition.x = self._initPos.x;
	end

  if (yMovement ~= 0) then
    yMovement = yMovement * totalMovement;
    calcPosition.y =  self._initPos.y + yMovement *  self.bobbingAmount;
  else 
    calcPosition.y =  self._initPos.y;
  end

  self.entity.transform:SetPosition(Vector3.Lerp(self.entity.transform:GetPosition(), calcPosition, dt * self.bobMultiplier));
end

