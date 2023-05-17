function bits(v)
	io.write(tostring(v)..' = ')
	for i = 15, 0, -1 do
		io.write(tostring((v >> i) & 1))
	end
	io.write('b\n')
end
