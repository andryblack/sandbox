-- preprocess, based on http://lua-users.org/wiki/SimpleLuaPreprocessor

local M = {}

function M.preprocess(file,defs,fn)
  local chunk = {n=0}
  local funcs = {}
  local output = {n=0}
  function funcs._put(l,s)
    while(#output<l) do
      table.insert(output,'--')
    end
    table.insert(output,s)
  end
  local mt = setmetatable(funcs,{__index=defs})
  local line_n = 0
  for line in file:lines() do
     if string.find(line, "^#") then
      table.insert(chunk, string.sub(line, 2))
     else
      table.insert(chunk,string.format('_put (%d,%q) ',line_n, line))
    end
    line_n = line_n + 1
  end
  assert(load(table.concat(chunk,'\n'),'pp:'..(fn or 'file'),'t',mt))()
  return table.concat(output,'\n')
end

return M