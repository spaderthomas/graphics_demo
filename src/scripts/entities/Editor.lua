local GLFW = require('glfw')
local inspect = require('inspect')

local EditState = {
  Idle = 'Idle',
  Insert = 'Insert',
  Edit = 'Edit',
  Drag = 'Drag',
  RectangleSelect = 'RectangleSelect',
  ReadyToDrawGeometry = 'ReadyToDrawGeometry',
  DrawingGeometry = 'DrawingGeometry',
}

Editor = tdengine.entity('Editor')
function Editor:init()
  self:add_component('Debug')
  self:add_component('Input')

  self.options = {
	 show_bounding_boxes = false,
	 show_minkowksi = false,
	 show_imgui_demo = false,
	 show_framerate = false
  }
  
  self.selected = nil
  self.state = EditState.Idle

  -- Stored as screen coordinates, converted to world when we submit the geometry
  self.geometry_start = { x = 0, y = 0 }
  
  self.filter = imgui.TextFilter.new()

  self.display_framerate = 0
  self.average_framerate = 0
  self.frame = 0

  local input = self:get_component('Input')
  input:set_channel(tdengine.InputChannel.Editor)
  input:enable()
end

function Editor:update(dt)
  local dbg = self:get_component('Debug')
  local input = self:get_component('Input')
  self:calculate_framerate()
  
  self:handle_input()
  
  imgui.SetNextWindowSize(300, 300)
  imgui.Begin("tded v2.0", true)
  imgui.Text('frame: ' .. tostring(self.frame))
  imgui.Text('fps: ' .. tostring(self.display_framerate))
  if imgui.Button("save imgui.ini") then
  	 tdengine.internal.save_imgui_layout()
  end

  imgui.Text(tostring(tdengine.get_camera_x()))
  imgui.Text(tostring(tdengine.get_camera_y()))
  
  imgui.Begin("scene", true)
  self:draw_entity_viewer()
  imgui.Separator()
  self:draw_tools()
  imgui.End()
  imgui.End()

  if self.state == EditState.ReadyToDrawGeometry then
  	 if input:was_pressed(GLFW.Keys.MOUSE_BUTTON_1) then
	 	self.geometry_start.x = tdengine.get_cursor_x()
	 	self.geometry_start.y = tdengine.get_cursor_y()
		self.state = EditState.DrawingGeometry
	 end
  elseif self.state == EditState.DrawingGeometry then
     if input:is_down(GLFW.Keys.MOUSE_BUTTON_1) then
	    local rect = self:get_rect()
	    tdengine.draw.rect_outline_screen(rect, tdengine.colors.red)
  	 else
		local box = self:create_entity('Box')
		local rect = self:get_rect()
		
		local aabb = box:get_component('BoundingBox')
		aabb.extents = rect.extents
		
		local position = box:get_component('Position')
		position.world = tdengine.screen_to_world(rect.origin)
		
		tdengine.internal.register_collider(box:get_id())

		self.state = EditState.Idle
		self.geometry_start.x = 0
		self.geometry_start.y = 0
	 end
  end
end

function Editor:calculate_framerate()
   local framerate = tdengine.framerate or 0
   self.average_framerate = self.average_framerate * .5
   self.average_framerate = self.average_framerate + framerate * .5
   self.frame = self.frame + 1
   if self.frame % 20 == 0 then
	  self.display_framerate = self.average_framerate
   end   
end

function Editor:handle_input()
  self:adjust_camera()
  
  local input = self:get_component('Input')
  if input:was_pressed(GLFW.Keys.ESCAPE) then
	self.selected = nil
	self.state = EditState.Idle
  end

  if input:was_pressed(GLFW.Keys.LEFT_CONTROL) then
    tdengine.internal.toggle_console()
  end
end

function Editor:adjust_camera()
  local input = self:get_component('Input')

  local offset = { x = 0, y = 0 }
  if input:is_down(GLFW.Keys.W) then
	offset.y = offset.y - .02
  end
  
  if input:is_down(GLFW.Keys.A) then
	offset.x = offset.x + .02
  end
  
  if input:is_down(GLFW.Keys.S) then
	offset.y = offset.y + .02
  end
  
  if input:is_down(GLFW.Keys.D) then
	offset.x = offset.x - .02
  end

  tdengine.move_camera(offset.x, offset.y)
end

function Editor:draw_tools()
  if imgui.Button("Add Geometry") then
  	 local input = self:get_component('Input')
	 self.state = EditState.ReadyToDrawGeometry
  end
  if imgui.Button("Save") then
     local x = 0
  end

end

function Editor:draw_entity_viewer()
  self.filter:Draw("Filter by name")
  for id, entity in pairs(Entities) do
	local name = entity:get_name()
	if self.filter:PassFilter(name) then
	  imgui.extensions.Entity(entity)
	end
  end
end

function Editor:get_rect()
   return {
      extents = {
	     x = math.abs(tdengine.get_cursor_x() - self.geometry_start.x),
		 y = math.abs(tdengine.get_cursor_y() - self.geometry_start.y)
	  },
	  origin = {
	     x = (tdengine.get_cursor_x() + self.geometry_start.x) / 2,
	     y = (tdengine.get_cursor_y() + self.geometry_start.y) / 2
	  }
   }
end