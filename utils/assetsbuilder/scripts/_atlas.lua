

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
	self.start_w = w
	self.start_h = h
	self.images = {}
	self.rects = { {0,0,w,h} }
end
function Atlas:add_image( img )
	table.insert(self.images,img)
	img.idx = #self.images
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
function Atlas:build_impl( ... )
	local res,err = pcall(function()
		for _,v in ipairs(self.images) do
			self:place_image(v)
		end
	end)
	return res,err
end
local function image_sorter(a,b)
	local pa = a.height + a.width
	local pb = b.height + b.width
	if pa == pb then
		return a.idx < b.idx
	end
	return pa > pb
end

function Atlas:build(  )
	table.sort(self.images, image_sorter)
	while true do
		local res,err = self:build_impl()
		if res then
			break
		end
		if err ~= not_found_place then
			self:dump()
			error(err)
		end
		if self.fixed_size then
			error('atlas build failed ' .. self.width .. 'x' .. self.height)
		end
		if self.width > self.height then
			self.height = self.height * 2
		else
			self.width = self.width * 2
		end
		self.rects = { {0,0,self.width,self.height} }
	end
	-- shunk down
	if not self.fixed_size and ( self.width > self.start_w or self.height > self.start_h) then
		local last_success = nil
		while true do
			last_success = {self.width,self.height}
			if self.width > self.height then
				self.width = self.width - 16
			else
				self.height = self.height - 16
			end
			self.rects = { {0,0,self.width,self.height} }
			local res,err = self:build_impl()
			if not res then
				self.width = last_success[1]
				self.height = last_success[2]

				self.rects = { {0,0,self.width,self.height} }
				assert(self:build_impl())
				break
			end
		end
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