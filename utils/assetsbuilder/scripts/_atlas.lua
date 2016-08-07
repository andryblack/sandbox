

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
	self.rects = { {0,0,w,h} }
end
function Atlas:add_image( img )
	table.insert(self.images,img)
end
function Atlas:find_best( img )
	local best = nil
	for _,r in ipairs(self.rects) do
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
local not_found_place = {}
function Atlas:place_image( img )
	local r = self:find_best( img )
	if r then
		-- subdiv
		
		if (r[3]-img.width) > (r[4]-img.height) then
			-- i   i ---
			--     | 1 
			-- i---i
			-- 2   | 
			table.insert(self.rects,{r[1]+img.width,r[2],r[3]-img.width,r[4]})
			table.insert(self.rects,{r[1],r[2]+img.height,img.width,r[4]-img.height})
		else
			-- i   i ---
			--     | 1 
			-- i---i ---
			-- 2    
			table.insert(self.rects,{r[1]+img.width,r[2],r[3]-img.width,img.height})
			table.insert(self.rects,{r[1],r[2]+img.height,r[3],r[4]-img.height})
		end
		r[3]=0
		r[4]=0
		img.place_to = {r[1],r[2]}
	else
		error(not_found_place)
	end
end
function Atlas:build(  )
	table.sort(self.images, function(a,b) return (a.height + a.width) > (b.height + b.width) end)
	while true do
		local res,err = pcall(function()
			for _,v in ipairs(self.images) do
				self:place_image(v)
			end
		end)
		if res then
			break
		end
		if err ~= not_found_place then
			self:dump()
			error(err)
		end
		if self.width > self.height then
			self.height = self.height * 2
		else
			self.width = self.width * 2
		end
		self.rects = { {0,0,self.width,self.height} }
	end
end

function Atlas:dump(  )
	print('atlas',self.width,self.height)
	for _,v in ipairs(self.images) do
		if v.place_to then
			print('placed:',v.place_to[1],v.place_to[2],v.width,v.height)
		else
			print('not placed:',v.width,v.height)
		end
	end
end


_M.Atlas = Atlas

return _M