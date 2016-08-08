
-- ported premake functions

function iif(condition, trueValue, falseValue)
	if condition then
		return trueValue
	else
		return falseValue
	end
end

local function override( t, name, func )
	local base = t[name]
	t[name] = function( ... )
		return func(base,...)
	end
end 

function string.capitalized(self)
	return self:gsub("^%l", string.upper)
end

function string.contains(s, match)
	return string.find(s, match, 1, true) ~= nil
end

function string.explode(s, pattern, plain)
	if (pattern == '') then return false end
	local pos = 0
	local arr = { }
	for st,sp in function() return s:find(pattern, pos, plain) end do
		table.insert(arr, s:sub(pos, st-1))
		pos = sp + 1
	end
	table.insert(arr, s:sub(pos))
	return arr
end

function string.findlast(s, pattern, plain)
	local curr = 0
	repeat
		local next = s:find(pattern, curr + 1, plain)
		if (next) then curr = next end
	until (not next)
	if (curr > 0) then
		return curr
	end
end

function string.lines(s)
	local trailing, n = s:gsub('.-\n', '')
	if #trailing > 0 then
		n = n + 1
	end
	return n
end

function path.appendExtension(p, ext)
	-- if the extension is nil or empty, do nothing
	if not ext or ext == "" then
		return p
	end

	-- if the path ends with a quote, pull it off
	local endquote
	if p:endswith('"') then
		p = p:sub(1, -2)
		endquote = '"'
	end

	-- add the extension if it isn't there already
	if not p:endswith(ext) then
		p = p .. ext
	end

	-- put the quote back if necessary
	if endquote then
		p = p .. endquote
	end

	return p
end

path.appendextension = path.appendExtension

function path.getbasename(p)
	local name = path.getname(p)
	local i = name:findlast(".", true)
	if (i) then
		return name:sub(1, i - 1)
	else
		return name
	end
end

function path.getdirectory(p)
	local i = string.findlast(p, "/", true)
	if (i) then
		if i > 1 then i = i - 1 end
		return p:sub(1, i)
	else
		return "."
	end
end

function path.getextension(p)
	local i = string.findlast(p, ".", true)
	if (i) then
		return p:sub(i)
	else
		return ""
	end
end

function path.getname(p)
	local i = string.findlast(p, "[/\\]")
	if (i) then
		return p:sub(i + 1)
	else
		return p
	end
end

function path.hasextension(fname, extensions)
	local fext = path.getextension(fname):lower()
	if type(extensions) == "table" then
		for _, extension in pairs(extensions) do
			if fext == extension then
				return true
			end
		end
		return false
	else
		return (fext == extensions)
	end
end

function path.rebase(p, oldbase, newbase)
	p = path.getabsolute(path.join(oldbase, p))
	p = path.getrelative(newbase, p)
	return p
end

function path.replaceextension(p, newext)
	local ext = path.getextension(p)

	if not ext then
		return p
	end

	if not string.findlast(newext, ".", true) then
		newext = "."..newext
	end

	return p:match("^(.*)"..ext.."$")..newext
end

function path.wildcards(pattern)
	-- Escape characters that have special meanings in Lua patterns
	pattern = pattern:gsub("([%+%.%-%^%$%(%)%%])", "%%%1")

	-- Replace wildcard patterns with special placeholders so I don't
	-- have competing star replacements to worry about
	pattern = pattern:gsub("%*%*", "\001")
	pattern = pattern:gsub("%*", "\002")

	-- Replace the placeholders with their Lua patterns
	pattern = pattern:gsub("\001", ".*")
	pattern = pattern:gsub("\002", "[^/]*")

	return pattern
end

override(os, "isdir", function(base, p)
	p = path.normalize(p)
	return base(p)
end)

override(os, "isfile", function(base, p)
	p = path.normalize(p)
	return base(p)
end)


function os.match(mask, matchFiles)
	-- Strip any extraneous weirdness from the mask to ensure a good
	-- match against the paths returned by the OS. I don't know if I've
	-- caught all the possibilities here yet; will add more as I go.

	mask = path.normalize(mask)

	-- strip off any leading directory information to find out
	-- where the search should take place

	local basedir = mask
	local starpos = mask:find("%*")
	if starpos then
		basedir = basedir:sub(1, starpos - 1)
	end
	basedir = path.getdirectory(basedir)
	if basedir == "." then
		basedir = ""
	end

	--print('os.match',basedir)
	-- recurse into subdirectories?
	local recurse = mask:find("**", nil, true)

	-- convert mask to a Lua pattern
	mask = path.wildcards(mask)

	local result = {}

	local function matchwalker(basedir)
		local wildcard = path.join(basedir, "*")

		-- retrieve files from OS and test against mask
		local m = os.matchstart(wildcard)
		while os.matchnext(m) do
			local isfile = os.matchisfile(m)
			if (matchFiles and isfile) or (not matchFiles and not isfile) then
				local fname = os.matchname(m)
				if isfile or not string.startswith(fname, ".") then
					fname = path.join(basedir, fname)
					if fname:match(mask) == fname then
						table.insert(result, fname)
					end
				end
			end
		end
		os.matchdone(m)

		-- check subdirectories
		if recurse then
			m = os.matchstart(wildcard)
			while os.matchnext(m) do
				if not os.matchisfile(m) then
					local dirname = os.matchname(m)
					if (not string.startswith(dirname, ".")) then
						matchwalker(path.join(basedir, dirname))
					end
				end
			end
			os.matchdone(m)
		end
	end

	matchwalker(basedir)
	return result
end

function os.matchdirs(mask)
	return os.match(mask, false)
end

function os.matchfiles(mask)
	return os.match(mask, true)
end

override(os,'mkdir',function(base,p)
	p = path.normalize(p)

	local dir = iif(string.startswith(p, "/"), "/", "")
	for part in p:gmatch("[^/]+") do
		dir = dir .. part

		if (part ~= "" and not path.isabsolute(part) and not os.isdir(dir)) then
			local ok, err = base(dir)
			if (not ok) then
				return nil, err
			end
		end

		dir = dir .. "/"
	end

	return true
end)

override(os,'remove',function(base,f)
	-- in case of string, just match files
	if type(f) == "string" then
		local p = os.matchfiles(f)
		for _, v in pairs(p) do
			local ok, err = base(v)
			if not ok then
				return ok, err
			end
		end
	-- in case of table, match files for every table entry
	elseif type(f) == "table" then
		for _, v in pairs(f) do
			local ok, err = os.remove(v)
			if not ok then
				return ok, err
			end
		end
	end
	return true
end)

override(os,'rmdir',function ( base,p )
	print('rmdir',p)
	-- recursively remove subdirectories
	local dirs = os.matchdirs(p .. "/*")
	for _, dname in ipairs(dirs) do
		os.rmdir(dname)
	end

	-- remove any files
	local files = os.matchfiles(p .. "/*")
	for _, fname in ipairs(files) do
		os.remove(fname)
	end

	-- remove this directory
	return base(p)
end)

override(os, "stat", function(base, p)
	p = path.normalize(p)
	return base(p)
end)


function os.check_file_new( src, dst )
	local s1 = assert(os.stat(src))
	local s2 = os.stat(dst)
	if not s2 then return true end
	return s1.mtime > s2.mtime
end

function load_sandbox( file , mt, init_data )
	local data = init_data or {}
    setmetatable(data,{__index=mt or {}})
    local f,e = loadfile(file,"t",data)
    if not f then error(e, 2) end
    if type(f) ~= "function" then
    	error("expected function, got " .. type(f) .. "(" .. tostring(f) .. ")" ,2)
    end
    local res = f()
    --setmetatable(data,nil)
    return data,res
end




