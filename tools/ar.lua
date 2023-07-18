-- luajit source
-- compiles lua files and archives them into one c file in stdout! yay!

local err = io.stderr
local out = io.stdout

function print(...) 
	for i = 1, select('#', ...) do
		local s = select(i, ...)
		io.stderr:write(tostring(s))
		io.stderr:write('\t')
	end
	io.stderr:write('\n')
end

local buff = ""

local function readfile(name)
	local f = io.open(name)
	if not f then
		print("Error : can't open file!")
		os.exit(5)
	end
	local s = f:read("a")
	f:close()
	return s
end

local function compile(code)
	local f, err = load(code)
	if not f then
		print(err)
		os.exit(6)
	end
	return string.dump(f)
end

local function better_name(name)
	return name:gsub('[^%w_]', '')
end

local function toccode(name, data)
	name = better_name(name)
	buff = buff .. string.format("unsigned char %s[] = {\n\t", name)
	for i = 1, #data do
		buff = buff .. string.byte(data, i, i) .. (i ~= #data and ', ' or '')
		if i % 8 == 7 then
			buff = buff .. '\n\t'
		end
	end
	buff = buff .. "};\n"
end	

for i = 1, #arg do
	out:write(readfile(arg[i]))
end
