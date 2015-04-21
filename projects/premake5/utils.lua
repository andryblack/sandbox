
local _M = {}

 function _M.append_path( path, files ) 
	local f = {}
	for _,v in ipairs(files) do
		f[#f+1] = path .. v
	end
	return f
end

return _M