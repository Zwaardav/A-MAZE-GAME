--[[
##########################################################################
  L E V E L   E D I T O R

 Health warning:  Not much work has gone into making this code beautiful.
                  Read it at your own risk.
##########################################################################

for k = 1, 16 do
	print("warptriggerdata[" .. k .. "][1], warptriggerdata[" .. k .. "][2], ")
end
]]

function love.load()
	rows = {}
	for r = 1, 14 do
		table.insert(rows, {64,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,64})
	end
	
	opentriggerdata = {}
	for r = 1, 16 do
		table.insert(opentriggerdata, 0)
	end
	
	warptriggerdata = {}
	for r = 1, 16 do
		table.insert(warptriggerdata, {0,0})
	end
	
	enemies = {}
	
	playerstartx, playerstarty = 0, 0
	
	scroll = 0
	
	edittrigger = 1
	
	ticker = 0
	
	hltileplus, hlx, hly = -1, -1, -1  -- destination highlights
	
	normalfont = love.graphics.newFont(12)
	almostnormalfont = love.graphics.newFont(11)
	smallfont = love.graphics.newFont(8)
	love.graphics.setFont(normalfont)
	
	levelname = "(unnamed level)"
	levelnumber = "*"
	
	keypressed = false
	showenemies = true
end

function love.draw()
	love.graphics.setFont(almostnormalfont)
	love.graphics.printf("Level editor for that asm maze game\n\nIf you let enemies walk over triggers, the triggers will stop showing.\nEnemies don't care about walls, they honor redirectors\n\n===\n\nHotkeys:\nClick L/R: Wall/air\nE: Add/Rotate enemy\nLShift+E: Remove enemy\nR: Add/Rotate enemy redirector\n-: Height -\n=: Height +\nQ: Scroll up\nA: Scroll down\nP: Set player start\nLCtrl+F: Fill all\nLCtrl+LShift+F: Unfill all\nF: Finish\nB: Barrier\nI: Show/Hide enemies\nC: Copy level to clipboard\nLCtrl+V: Load level from clipboard\n\n===\n\nData:\nHeight: " .. #rows .. " (" .. ((#rows-2)/6) .. " screens)\nEnemies: " .. #enemies .. "/255\nLoaded level#: " .. levelnumber .. "\nLoaded name: " .. levelname .. "\n\n===\n\nTriggers:\n[F1] < " .. edittrigger .. "/16 > [F2]\n\n[F3 set TARGET] Opener " .. edittrigger .. " (t" .. (31+edittrigger) .. ") (t+" .. opentriggerdata[edittrigger] .. ")\n[F4 set TARGET] Warp " .. edittrigger .. " (t" .. (47+edittrigger) .. ") (to " .. warptriggerdata[edittrigger][1] .. "," .. warptriggerdata[edittrigger][2] .. ")\n\nO: Place opener block " .. edittrigger .. "\nW: Place warp block " .. edittrigger, 0,0,love.graphics.getWidth(), "right")
	love.graphics.setFont(normalfont)
	
	hoveranywhere = false
	
	for kr, vr in pairs(rows) do
		for kc, vc in pairs(vr) do
			if vc == 0 then -- air
				love.graphics.rectangle("line", 16*kc+.5, 16*kr+.5+scroll, 16-1, 16-1)
			elseif vc >= 1 and vc <= 4 then -- enemy redirector
				love.graphics.setColor(255,192,192)
				love.graphics.rectangle("fill", 16*kc, 16*kr+scroll, 16, 16)
				love.graphics.setColor(128,128,128)
				love.graphics.rectangle("line", 16*kc+.5, 16*kr+.5+scroll, 16-1, 16-1)
				love.graphics.setColor(0,0,0)
				love.graphics.print("R" .. ("^v<>"):sub(vc,vc), 16*kc, 16*kr+scroll)
				love.graphics.setColor(255,255,255)
			elseif vc >= 32 and vc <= 47 then -- opener trigger
				love.graphics.setColor(0,64,255)
				love.graphics.rectangle("fill", 16*kc, 16*kr+scroll, 16, 16)
				love.graphics.setColor(128,128,128)
				love.graphics.rectangle("line", 16*kc+.5, 16*kr+.5+scroll, 16-1, 16-1)
				love.graphics.setColor(255,255,255)
				love.graphics.setFont(smallfont)
				love.graphics.print("o" .. vc-31, 16*kc, 16*kr+scroll)
				love.graphics.setFont(normalfont)
				
				if mouseontile(kc, kr) then
					hltileplus = opentriggerdata[vc-31]
					hlx, hly = -1, -1
				end
			elseif vc >= 48 and vc <= 63 then -- warp trigger
				love.graphics.setColor(0,192,255)
				love.graphics.rectangle("fill", 16*kc, 16*kr+scroll, 16, 16)
				love.graphics.setColor(128,128,128)
				love.graphics.rectangle("line", 16*kc+.5, 16*kr+.5+scroll, 16-1, 16-1)
				love.graphics.setColor(255,255,255)
				love.graphics.setFont(smallfont)
				love.graphics.print("w" .. vc-47, 16*kc, 16*kr+scroll)
				love.graphics.setFont(normalfont)
				
				if mouseontile(kc, kr) then
					hlx, hly = unpack(warptriggerdata[vc-47])
					hltileplus = -1
				end
			elseif vc == 64 then -- wall
				love.graphics.rectangle("fill", 16*kc, 16*kr+scroll, 16, 16)
				love.graphics.setColor(128,128,128)
				love.graphics.rectangle("line", 16*kc+.5, 16*kr+.5+scroll, 16-1, 16-1)
				love.graphics.setColor(255,255,255)
			elseif vc == 65 then -- barrier
				love.graphics.setColor(128,64,0)
				love.graphics.rectangle("fill", 16*kc, 16*kr+scroll, 16, 16)
				love.graphics.setColor(128,128,128)
				love.graphics.rectangle("line", 16*kc+.5, 16*kr+.5+scroll, 16-1, 16-1)
				love.graphics.setColor(255,255,255)
				love.graphics.print("#", 16*kc, 16*kr+scroll)
			elseif vc == 96 then -- finish
				love.graphics.setColor(0,200,0)
				love.graphics.rectangle("fill", 16*kc, 16*kr+scroll, 16, 16)
				love.graphics.setColor(255,255,255)
				love.graphics.print("F", 16*kc, 16*kr+scroll)
			else
				love.graphics.setColor(255,0,128)
				love.graphics.rectangle("fill", 16*kc, 16*kr+scroll, 16, 16)
				love.graphics.setColor(255,255,255)
			end
			
			if kc == playerstartx and kr == playerstarty then
				love.graphics.setColor(0,255,0)
				love.graphics.rectangle("fill", 16*kc+4, 16*kr+4+scroll, 8, 8)
				love.graphics.setColor(255,255,255)
			end
			
			if (hltileplus == (kr-1)*16+(kc-1) or (hlx == (kc-1) and hly == (kr-1))) and ticker < 15 then
				love.graphics.setColor(255,0,0)
				love.graphics.rectangle("fill", 16*kc, 16*kr+scroll, 16, 16)
				love.graphics.setColor(255,255,255)
				love.graphics.print("T", 16*kc, 16*kr+scroll)
			end
			
			if kc == 16 and ((kr-1)/6) == math.floor(((kr-1)/6)) then
				love.graphics.print("A", 16*17, 16*kr+scroll)
			elseif kc == 16 and ((kr-2)/6) == math.floor(((kr-2)/6)) then
				love.graphics.print("V " .. ((kr-2)/6), 16*17, 16*kr+scroll)
			end
			
			if mouseontile(kc, kr) then
				hoveranywhere = true
				if love.mouse.isDown("l") or love.mouse.isDown(1) then
					rows[kr][kc] = 64
				elseif love.mouse.isDown("r") or love.mouse.isDown(2) then
					rows[kr][kc] = 0
				end
				
				if love.keyboard.isDown("p") then
					playerstartx, playerstarty = kc, kr
				elseif love.keyboard.isDown("f") and not love.keyboard.isDown("lctrl") then
					rows[kr][kc] = 96
				elseif love.keyboard.isDown("b") then
					rows[kr][kc] = 65
				elseif love.keyboard.isDown("r") and not keypressed then
					keypressed = true
					rows[kr][kc] = (rows[kr][kc]+1)%5
				elseif love.keyboard.isDown("e") and not keypressed then
					keypressed = true
				
					if love.keyboard.isDown("lshift") then
						-- Removing
						for k,v in pairs(enemies) do
							if v[1] == kc and v[2] == kr then
								table.remove(enemies, k)
								break
							end
						end
					else
						-- Adding, rotating. Is there already an enemy there?
						local theAnswerIsYES = false
						for k,v in pairs(enemies) do
							if v[1] == kc and v[2] == kr then
								theAnswerIsYES = true
								
								-- Rotate it now
								enemies[k][3] = (enemies[k][3]+1)%5
							end
						end
						if not theAnswerIsYES then
							table.insert(enemies, {kc, kr, 1})
						end
					end
				elseif love.keyboard.isDown("f3") then
					opentriggerdata[edittrigger] = (kr-1)*16+(kc-1)
					hltileplus = (kr-1)*16+(kc-1)
					hlx, hly = -1, -1
				elseif love.keyboard.isDown("f4") then
					warptriggerdata[edittrigger][1], warptriggerdata[edittrigger][2] = kc-1, kr-1
					hltileplus = -1
					hlx, hly = kc-1, kr-1
				elseif love.keyboard.isDown("o") then
					rows[kr][kc] = 31+edittrigger
				elseif love.keyboard.isDown("w") then
					rows[kr][kc] = 47+edittrigger
				end
				
				love.graphics.print("Tile +" .. ((kr-1)*16+(kc-1)) .. " [" .. (kc-1) .. "," .. (kr-1) .. "]", love.graphics.getWidth()-125, love.graphics.getHeight()-20)
			end
		end
	end
	
	-- Now display all the enemies, if shown!
	if showenemies then
		love.graphics.setColor(255,128,128)
		for k,v in pairs(enemies) do
			love.graphics.print("E" .. ("-^v<>"):sub(v[3]+1, v[3]+1), 16*v[1], 16*v[2]+scroll)
		end
		love.graphics.setColor(255,255,255)
	end
	
	if not hoveranywhere then
		hltileplus, hlx, hly = -1, -1, -1
	end
end

function love.update(dt)
	if love.keyboard.isDown("q") then
		scroll = scroll + 16
	elseif love.keyboard.isDown("a") then
		scroll = scroll - 16
	end
	
	ticker = (ticker + 120*dt) % 30
end

-- .
function loadlevelstring(lvlstr)
	rows = {}
	k = 0
	for line in string.gmatch(lvlstr,'[^\r\n]+') do
		k = k + 1
		if k == 1 then
			local checker                                                                                                                                -- #     ht     px     py     #o     #w     #e
			checker, levelnumber, _, playerstartx, playerstarty, AMOUNTOPENERS, AMOUNTWARPS, AMOUNTENEMIES, levelname = line:match("level(%d+)Data: %.db (%d+), (%d+), (%d+), (%d+), (%d+), (%d+), (%d+), 0,\"(.*)\",0")
			
			assert(checker ~= nil, "That wasn't valid level data I think, line 1")
			
			playerstartx = playerstartx + 1
			playerstarty = playerstarty + 1
			
			AMOUNTOPENERS = tonumber(AMOUNTOPENERS)
			AMOUNTWARPS = tonumber(AMOUNTWARPS)
			AMOUNTENEMIES = tonumber(AMOUNTENEMIES)
		elseif k == 2 then
			for opent = 1, 16 do
				opentriggerdata[opent] = 0
			end
			if line ~= "\t; no openers" then
				--theopenerthings = {line:match("\t%.db (%d+),(%d+)" .. (", (%d+),(%d+)"):rep(AMOUNTOPENERS-1))}
				theopenerthings = {line:match("\t%.dw (%d+)" .. (", (%d+)"):rep(AMOUNTOPENERS-1))}
				
				for opent = 0, (AMOUNTOPENERS-1) do
					--opentriggerdata[opent+1] = theopenerthings[opent*2+1] + theopenerthings[opent*2+2]*256
					opentriggerdata[opent+1] = theopenerthings[opent+1]
				end
				
				-- And now convert all that shit from strings to numbers
				for opent = 1, 16 do
					opentriggerdata[opent] = tonumber(opentriggerdata[opent])
				end
				
				print("OPENERS: " .. AMOUNTOPENERS .. "\n" .. table.concat(opentriggerdata, ", "))
			else
				print("NO OPENERS IMPORTED")
			end
		elseif k == 3 then
			warptriggerdata = {}
			for r = 1, 16 do
				table.insert(warptriggerdata, {0,0})
			end
			
			if line ~= "\t; no warps" then
				warptriggerdata[1][1], warptriggerdata[1][2], warptriggerdata[2][1], warptriggerdata[2][2], warptriggerdata[3][1], warptriggerdata[3][2], warptriggerdata[4][1], warptriggerdata[4][2], warptriggerdata[5][1], warptriggerdata[5][2], warptriggerdata[6][1], warptriggerdata[6][2], warptriggerdata[7][1], warptriggerdata[7][2], warptriggerdata[8][1], warptriggerdata[8][2], warptriggerdata[9][1], warptriggerdata[9][2], warptriggerdata[10][1], warptriggerdata[10][2], warptriggerdata[11][1], warptriggerdata[11][2], warptriggerdata[12][1], warptriggerdata[12][2], warptriggerdata[13][1], warptriggerdata[13][2], warptriggerdata[14][1], warptriggerdata[14][2], warptriggerdata[15][1], warptriggerdata[15][2], warptriggerdata[16][1], warptriggerdata[16][2] = line:match("\t%.db (%d+),(%d+)" .. (", (%d+),(%d+)"):rep(AMOUNTWARPS-1))
				
				-- What remains? (16-(AMOUNTWARPS-1))
				if AMOUNTOPENERS < 16 then
					for nilopener = (AMOUNTWARPS+1), 16 do
						warptriggerdata[nilopener][1], warptriggerdata[nilopener][2] = 0,0
					end
				end
				
				-- Numbers
				for warpt = 1, 16 do
					warptriggerdata[warpt][1], warptriggerdata[warpt][2] = tonumber(warptriggerdata[warpt][1]), tonumber(warptriggerdata[warpt][2])
				end
				
				print("WARPS: " .. AMOUNTWARPS)
				
				for k,v in pairs(warptriggerdata) do
					print(k .. ": " .. anythingbutnil(v[1]) .. "," .. anythingbutnil(v[2]))
				end
			else
				print("NO WARPS IMPORTED")
			end
		elseif k == 4 then
			enemies = {}
			
			if line ~= "\t; no enemies" then
				theenemies = {line:match("\t%.db 0,0,0,(%d+),(%d+),(%d+)" .. (", 0,0,0,(%d+),(%d+),(%d+)"):rep(AMOUNTENEMIES-1))}
				
				for currentenemy = 0, AMOUNTENEMIES-1 do
					table.insert(enemies, {tonumber(theenemies[3*currentenemy+1])+1, tonumber(theenemies[3*currentenemy+2])+1, tonumber(theenemies[3*currentenemy+3])})
				end
			else
				print("NO ENEMIES IMPORTED")
			end
		else
			-- Just a row of tiles
			table.insert(rows, {line:match("\t.db (%d+), (%d+), (%d+), (%d+), (%d+), (%d+), (%d+), (%d+), (%d+), (%d+), (%d+), (%d+), (%d+), (%d+), (%d+), (%d+)")})
			
			-- Damn
			for k,v in pairs(rows[#rows]) do
				rows[#rows][k] = tonumber(v)
			end
		end
	end
end
-- .

function love.keypressed(key)
	if key == "c" then
		-- How many openers and warps do we have?
		AMOUNTOPENERS = 0
		for opt = 16, 1, -1 do
			if opentriggerdata[opt] ~= 0 then
				AMOUNTOPENERS = opt
				break
			end
		end
		
		AMOUNTWARPS = 0
		for wpt = 16, 1, -1 do
			if warptriggerdata[wpt][1] ~= 0 or warptriggerdata[wpt][2] ~= 0 then
				AMOUNTWARPS = wpt
				break
			end
		end
	
		local clipboardstring = ""
		
		if playerstartx == 0 or playerstarty == 0 then
			clipboardstring = clipboardstring .. "; Caution: player start point not set!\r\n"
		end
		
		clipboardstring = clipboardstring .. "level" .. levelnumber .. "Data: .db " .. levelnumber .. ", " .. #rows .. ", " .. (playerstartx-1) .. ", " .. (playerstarty-1) .. ", " .. AMOUNTOPENERS .. ", " .. AMOUNTWARPS .. ", " .. #enemies .. ", 0,\"" .. levelname .. "\",0\r\n"
		
		if AMOUNTOPENERS == 0 then
			clipboardstring = clipboardstring .. "\t; no openers\r\n"
		else
			clipboardstring = clipboardstring .. "\t.dw "
			for thing = 1, AMOUNTOPENERS do
				--clipboardstring = clipboardstring .. (opentriggerdata[thing]%256) .. "," .. math.floor(opentriggerdata[thing]/256)
				clipboardstring = clipboardstring .. opentriggerdata[thing]
				if thing < AMOUNTOPENERS then
					clipboardstring = clipboardstring .. ", "
				else
					clipboardstring = clipboardstring .. "\r\n"
				end
			end
		end
		
		if AMOUNTWARPS == 0 then
			clipboardstring = clipboardstring .. "\t; no warps"
		else
			clipboardstring = clipboardstring .. "\t.db "
			for thing = 1, AMOUNTWARPS do
				clipboardstring = clipboardstring .. warptriggerdata[thing][1] .. "," .. warptriggerdata[thing][2]
				if thing < AMOUNTWARPS then
					clipboardstring = clipboardstring .. ", "
				end
			end
		end
		
		if #enemies == 0 then
			clipboardstring = clipboardstring .. "\r\n\t; no enemies"
		else
			clipboardstring = clipboardstring .. "\r\n\t.db "
			for k,v in pairs(enemies) do
				clipboardstring = clipboardstring .. "0,0,0," .. (v[1]-1) .. "," .. (v[2]-1) .. "," .. v[3]
				if k < #enemies then
					clipboardstring = clipboardstring .. ", "
				end
			end
		end
		
		for kr, vr in pairs(rows) do
			clipboardstring = clipboardstring .. "\r\n\t.db " .. table.concat(vr, ", ")
		end
		
		love.system.setClipboardText(clipboardstring)
	elseif key == "v" and love.keyboard.isDown("lctrl") then
		loadlevelstring(love.system.getClipboardText())
	elseif key == "-" and #rows > 2 then
		for r = 1,6 do
			table.remove(rows, #rows)
		end
	elseif key == "=" then
		for r = 1,6 do
			table.insert(rows, {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0})
		end
	elseif key == "f" and love.keyboard.isDown("lctrl") then
		if love.keyboard.isDown("lshift") then
			for kr, vr in pairs(rows) do
				for kc, vc in pairs(vr) do
					rows[kr][kc] = 0
				end
			end
		else
			for kr, vr in pairs(rows) do
				for kc, vc in pairs(vr) do
					rows[kr][kc] = 64
				end
			end
		end
	elseif key == "f1" then
		if edittrigger == 1 then
			edittrigger = 16
		else
			edittrigger = edittrigger - 1
		end
	elseif key == "f2" then
		if edittrigger == 16 then
			edittrigger = 1
		else
			edittrigger = edittrigger + 1
		end
	elseif key == "escape" then
		hltileplus, hlx, hly = -1, -1, -1
	elseif key == "i" then
		showenemies = not showenemies
	end
end

function love.keyreleased()
	keypressed = false
end

function mouseon(x, y, w, h)
	-- Determines whether mouse is on a box with top left corner x1,y1 and width w and height h
	if (love.mouse.getX() >= x) and (love.mouse.getX() < x+w) and (love.mouse.getY() >= y) and (love.mouse.getY() < y+h) then
		return true
	else
		return false
	end
end

function mouseontile(x, y)
	return mouseon(x*16, y*16+scroll, 16, 16)
end

function anythingbutnil(this)
	if this == nil then
		return ""
	else
		return this
	end
end

function anythingbutnil0(this)
	-- Same, but instead of an empty string return 0
	if this == nil then
		return 0
	else
		return this
	end
end