require "audiere"
local device = audiere.OpenDevice()
local source = audiere.OpenSampleSource(arg[1])
for i=0,(source:getTagCount() - 1) do
  local key = source:getTagKey(i)
  local value = source:getTagValue(i)
  print(string.format("%s=%s", key, value))
end
local stream = audiere.OpenSound(device, source, true)
stream:play()
print("Press Enter to stop.")
io.read()
