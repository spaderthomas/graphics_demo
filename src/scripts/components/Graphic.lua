local inspect = require('inspect')

Graphic = tdengine.component('Graphic')
function Graphic:init()
   self.layer = 1

   self.parent:add_component("Animation")
end


function Graphic:update(dt)
  tdengine.draw_entity(self.parent)
end
