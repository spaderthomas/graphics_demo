Console::Console()
{
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;
	Commands.push_back("HELP");
	Commands.push_back("HISTORY");
	Commands.push_back("CLEAR");
	AddLog("Welcome to Dear ImGui!");
}
Console::~Console() {}
int   Console::Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
int   Console::Strnicmp(const char* str1, const char* str2, int n) {
	int d = 0;
	while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) {
		str1++;
		str2++;
		n--;
	}
	return d;
}
char* Console::Strdup(const char *str) { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }
void  Console::Strtrim(char* str) { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }
void  Console::ClearLog()
{
	for (int i = 0; i < Items.Size; i++)
		free(Items[i]);
	Items.clear();
	ScrollToBottom = true;
}
void  Console::AddLog(const char* fmt, ...) IM_FMTARGS(2)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	Items.push_back(Strdup(buf));
	ScrollToBottom = true;
}
void  Console::Draw(const char* title)
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title, NULL))
	{
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("INFO")) {
		for (auto& [key, value] : layer_map) {
			if (value == active_layer)
				ImGui::Text("Layer: %s", key.c_str());
		}
	}
	
	if (ImGui::SmallButton("Clear")) { ClearLog(); } ImGui::SameLine();
	bool copy_to_clipboard = ImGui::SmallButton("Copy"); ImGui::SameLine();
	if (ImGui::SmallButton("Scroll to bottom")) ScrollToBottom = true;
	
	ImGui::Separator();
	
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	static ImGuiTextFilter filter;
	filter.Draw("Filter", 180);
	ImGui::PopStyleVar();
	ImGui::Separator();
	
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::Selectable("Clear")) ClearLog();
		ImGui::EndPopup();
	}
	
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	if (copy_to_clipboard)
		ImGui::LogToClipboard();
	
	// Apply special colors
	ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
	for (int i = 0; i < Items.Size; i++)
	{
		const char* item = Items[i];
		if (!filter.PassFilter(item))
			continue;
		ImVec4 col = col_default_text;
		if (strstr(item, "[error]")) col = ImColor(1.0f, 0.4f, 0.4f, 1.0f);
		else if (strncmp(item, "# ", 2) == 0) col = ImColor(1.0f, 0.78f, 0.58f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, col);
		ImGui::TextUnformatted(item);
		ImGui::PopStyleColor();
	}
	if (copy_to_clipboard)
		ImGui::LogFinish();
	if (ScrollToBottom)
		ImGui::SetScrollHere(1.0f);
	ScrollToBottom = false;
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::Separator();
	
	// Command-line
	bool reclaim_focus = false;
	ImGui::SetKeyboardFocusHere(0);
	if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
	{
		Strtrim(InputBuf);
		if (InputBuf[0])
			ExecCommand(InputBuf);
		strcpy(InputBuf, "");
		reclaim_focus = true;
	}
	
	// Demonstrate keeping focus on the input box
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
	
	ImGui::End();
}
int   Console::TextEditCallbackStub(ImGuiTextEditCallbackData* data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
{
	Console* console = (Console*)data->UserData;
	return console->TextEditCallback(data);
}
int   Console::TextEditCallback(ImGuiTextEditCallbackData* data)
{
	//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
	switch (data->EventFlag)
	{
		case ImGuiInputTextFlags_CallbackCompletion:
		{
			// Example of TEXT COMPLETION
			
			// Locate beginning of current word
			const char* word_end = data->Buf + data->CursorPos;
			const char* word_start = word_end;
			while (word_start > data->Buf)
			{
				const char c = word_start[-1];
				if (c == ' ' || c == '\t' || c == ',' || c == ';')
					break;
				word_start--;
			}
			
			// Build a list of candidates
			ImVector<const char*> candidates;
			for (int i = 0; i < Commands.Size; i++)
				if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
				candidates.push_back(Commands[i]);
			
			if (candidates.Size == 0)
			{
				// No match
				AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
			}
			else if (candidates.Size == 1)
			{
				// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0]);
				data->InsertChars(data->CursorPos, " ");
			}
			else
			{
				// Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
				int match_len = (int)(word_end - word_start);
				for (;;)
				{
					int c = 0;
					bool all_candidates_matches = true;
					for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
						if (i == 0)
						c = toupper(candidates[i][match_len]);
					else if (c == 0 || c != toupper(candidates[i][match_len]))
						all_candidates_matches = false;
					if (!all_candidates_matches)
						break;
					match_len++;
				}
				
				if (match_len > 0)
				{
					data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
					data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
				}
				
				// List matches
				AddLog("Possible matches:\n");
				for (int i = 0; i < candidates.Size; i++)
					AddLog("- %s\n", candidates[i]);
			}
			
			break;
		}
		case ImGuiInputTextFlags_CallbackHistory:
		{
			// Example of HISTORY
			const int prev_history_pos = HistoryPos;
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (HistoryPos == -1)
					HistoryPos = History.Size - 1;
				else if (HistoryPos > 0)
					HistoryPos--;
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (HistoryPos != -1)
					if (++HistoryPos >= History.Size)
					HistoryPos = -1;
			}
			
			// A better implementation would preserve the data on the current input line along with cursor position.
			if (prev_history_pos != HistoryPos)
			{
				data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen = (int)snprintf(data->Buf, (size_t)data->BufSize, "%s", (HistoryPos >= 0) ? History[HistoryPos] : "");
				data->BufDirty = true;
			}
		}
	}
	return 0;
}
void  Console::ExecCommand(char* command_line)
{
	AddLog("# %s\n", command_line);
	
	// Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
	HistoryPos = -1;
	for (int i = History.Size - 1; i >= 0; i--)
		if (Stricmp(History[i], command_line) == 0)
	{
		free(History[i]);
		History.erase(History.begin() + i);
		break;
	}
	History.push_back(Strdup(command_line));

	// If this gets set, we know not to pipe the command down to the layer-specific handler
	bool ran_generic_command = false;

	auto copy = (char*)calloc(sizeof(command_line) + 1, sizeof(char));
	strcpy(copy, command_line);
	char* command = strtok(copy, " ");
	if (Stricmp(command, "editor") == 0) {
		ran_generic_command = true;
		show_console = false;
		active_layer = &editor;
		iactive_layer = EDITOR_IDX;
	}
	else if (Stricmp(command, "cutscene_thing") == 0) {
		ran_generic_command = true;
		show_console = false;
		active_layer = &cutscene_thing;
		iactive_layer = CUTSCENE_IDX;
	}
	else if (Stricmp(command, "game") == 0) {
		ran_generic_command = true;
		show_console = false;
		active_layer = &game;
		iactive_layer = GAME_IDX;
	}
	else if (Stricmp(command, "battle") == 0) {
		ran_generic_command = true;
		show_console = false;
		active_layer = &battle;
		iactive_layer = BATTLE_IDX;
	}
	else if (Stricmp(command, "reload") == 0) {
		ran_generic_command = true;
		active_layer->reload();
	} else if (Stricmp(command, "screen") == 0) {
		ran_generic_command = true;
		char* res = strtok(NULL, " ");
		if (!res) {
			AddLog("format: screen {640, 720, 1080, 1440}");
			return;
		}
		
		if (!strcmp(res, "640")) use_640_360();
		else if (!strcmp(res, "720")) use_720p();
		else if (!strcmp(res, "1080")) use_1080p();
		else if (!strcmp(res, "1440")) use_1440p();
		else AddLog("format: screen {640, 720, 1080, 1440}");
	}
	else if (Stricmp(command, "level") == 0) {
		ran_generic_command = true;
		char* which = strtok(NULL, " ");

		// Just because you always want the game and editor to be synced for sure
		if (active_layer == &game) {
			swap_level(&editor, which);
		}
		else if (active_layer == &editor) {
			swap_level(&game, which);
		}
		
		swap_level(active_layer, which);
	} else if (Stricmp(command, "state") == 0) {
		ran_generic_command = true;
		char* which = strtok(NULL, " ");
		bool value;
		istringstream(strtok(NULL, " ")) >> std::boolalpha >> value;
		update_state(which, value);
	} else if (Stricmp(command, "state?") == 0) {
		ran_generic_command = true;
		string which = strtok(NULL, " ");
		
		bool found = false;
		for (auto& [state_name, value] : game_state) {
			if (!Stricmp(state_name.c_str(), which.c_str())) {
				found = true;
				string message = game_state[which] ?
					which + " = true" :
					which + " = false";
				AddLog(message.c_str());
			}
		}

		if (!found) {
			string message = "Sorry, couldn't find the state named [" + which + "]";
			AddLog(message.c_str());
		}
	}
	
	
	if (!ran_generic_command) active_layer->exec_console_cmd(command_line);
}


