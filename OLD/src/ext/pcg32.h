/* Original code was taken from the Luau programming language and it was
 * licensed under MIT License; "lmathlib.cpp"
 * Modifications by UtoECat : wrapped to class.
 *
 * Copyright (C) 2023 UtoECat
 * Copyright (c) 2019-2022 Roblox Corporation
 * Copyright (c) 1994–2019 Lua.org, PUC-Rio.

 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this file and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without
 * limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following
 * conditions:
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * While this file is MIT, all others files in this project 
 * are likely GNU GPL!
 */

class PCGRandom {
	protected :
	static constexpr int increment = 105;
	uint64_t state;
	private:
	uint64_t nextState() {
		uint64_t old = state;
		state = old * 6364136223846793005ULL + (increment | 1);
		return old;
	}
	public :
	PCGRandom(uint64_t seed) {
		setseed(seed);
	}
	PCGRandom() : PCGRandom(0) {};
	~PCGRandom() = default;
	uint32_t generate() {
		uint64_t old = nextState();
		uint32_t sft = uint32_t(((old >> 18u) ^ old) >> 27u);
		uint32_t rot = uint32_t(old >> 59u);
		return (sft >> rot) | (sft << ((-int32_t(rot)) & 31));
	}
	void setseed(uint64_t seed) {
		state = 0;
		nextState();
		state += seed;
		nextState();
	}
	uint32_t operator()() {
		return generate();
	}
};
