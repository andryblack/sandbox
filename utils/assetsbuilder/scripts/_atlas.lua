

local _M = {}


local Atlas = {}
function Atlas.new( w, h )
	local a = setmetatable({},{__index=Atlas})
	a:_init( w, h )
	return a
end
function Atlas:_init( w, h )
	self.width = w
	self.height = h
	self.images = {}
	self.rects = {}
	self.rects[ {0,0,w,h} ] = true
end
function Atlas:add_image( img )
	table.insert(self.images,img)
end
function Atlas:find_best( img )
	local best = nil
	for r,_ in pairs(self.rects) do
		if r[3] >= img.width and r[4] >= img.height then
			if not best then 
				best = r
			else
				if r[3] < best[3] then
					best = r
				end
			end
		end
	end
	return best
end
function Atlas:place_image( img )
	local r = self:find_best( img )
	if r then
		-- subdiv
		self.rects[r] = nil
		self.rects[{r[1]+img.width,r[2],r[3]-img.width,r[4]}] = true
		self.rects[{r[1],r[2]+img.height,img.width,r[4]-img.height}] = true
		img.place_to = {r[1],r[2]}
	else
		error('not found place')
	end
end
function Atlas:build(  )
	table.sort(self.images, function(a,b) return a.height > b.height end)
	for _,v in ipairs(self.images) do
		self:place_image(v)
	end
end

function Atlas:dump(  )
	print('atlas',self.width,self.height)
	for _,v in ipairs(self.images) do
		print('placed:',v.place_to[1],v.place_to[2],v.width,v.height)
	end
end


_M.Atlas = Atlas

return _M