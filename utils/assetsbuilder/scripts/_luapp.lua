-- preprocess, based on http://lua-users.org/wiki/SimpleLuaPreprocessor

local M = {}
function M.preprocess(file,defs)
  local chunk = {n=0}
  local funcs = {}
  local output = {n=0}
  function funcs._put(s)
    table.insert(output,s)
  end
  local mt = setmetatable(funcs,{__index=defs})
  for line in file:lines() do
     if string.find(line, "^#") then
      table.insert(chunk, string.sub(line, 2))
     else
      table.insert(chunk,string.format('_put %q ', line))
    end
  end
  load(table.concat(chunk,'\n'),'pp','t',mt)()
  return table.concat(output,'\n')
end

return M