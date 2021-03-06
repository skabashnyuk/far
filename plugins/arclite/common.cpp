#include "utils.hpp"
#include "sysutils.hpp"
#include "farutils.hpp"
#include "error.hpp"
#include "common.hpp"
#include "farutils.hpp"

Error g_com_error;

unsigned calc_percent(unsigned __int64 completed, unsigned __int64 total) {
  unsigned percent;
  if (total == 0)
    percent = 0;
  else
    percent = round(static_cast<double>(completed) / total * 100);
  if (percent > 100)
    percent = 100;
  return percent;
}

unsigned __int64 get_module_version(const wstring& file_path) {
  unsigned __int64 version = 0;
  DWORD dw_handle;
  DWORD ver_size = GetFileVersionInfoSizeW(file_path.c_str(), &dw_handle);
  if (ver_size) {
    Buffer<unsigned char> ver_block(ver_size);
    if (GetFileVersionInfoW(file_path.c_str(), dw_handle, ver_size, ver_block.data())) {
      VS_FIXEDFILEINFO* fixed_file_info;
      UINT len;
      if (VerQueryValueW(ver_block.data(), L"\\", reinterpret_cast<LPVOID*>(&fixed_file_info), &len)) {
        return (static_cast<unsigned __int64>(fixed_file_info->dwFileVersionMS) << 32) + fixed_file_info->dwFileVersionLS;
      }
    }
  }
  return version;
}

unsigned __int64 parse_size_string(const wstring& str) {
  unsigned __int64 size = 0;
  unsigned i = 0;
  while (i < str.size() && str[i] >= L'0' && str[i] <= L'9') {
    size = size * 10 + (str[i] - L'0');
    i++;
  }
  while (i < str.size() && str[i] == L' ') i++;
  if (i < str.size()) {
    wchar_t mod_ch = str[i];
    unsigned __int64 mod_mul;
    if (mod_ch == L'K' || mod_ch == L'k') mod_mul = 1024;
    else if (mod_ch == L'M' || mod_ch == L'm') mod_mul = 1024 * 1024;
    else if (mod_ch == L'G' || mod_ch == L'g') mod_mul = 1024 * 1024 * 1024;
    else mod_mul = 1;
    size *= mod_mul;
  }
  return size;
}

// expand macros enclosed in question marks
wstring expand_macros(const wstring& text) {
  const wchar_t c_macro_sep = L'?';
  wstring result;
  size_t b_pos, e_pos;
  size_t pos = 0;
  while (true) {
    b_pos = text.find(c_macro_sep, pos);
    if (b_pos == wstring::npos)
      break;
    e_pos = text.find(c_macro_sep, b_pos + 1);
    if (e_pos == wstring::npos)
      break;

    wstring macro = L"print(" + text.substr(b_pos + 1, e_pos - b_pos - 1) + L") Enter";
    ActlKeyMacro akm;
    akm.Command = MCMD_POSTMACROSTRING;
    akm.Param.PlainText.SequenceText = macro.c_str();
    akm.Param.PlainText.Flags = 0;
    wstring mresult;
    if (Far::adv_control(ACTL_KEYMACRO, &akm))
      Far::input_dlg(wstring(), wstring(), mresult);
    else
      FAIL(E_ABORT);

    result.append(text.data() + pos, b_pos - pos);
    result.append(mresult);
    pos = e_pos + 1;
  }
  result.append(text.data() + pos, text.size() - pos);
  return result;
}

#define CP_UTF16 1200
wstring load_file(const wstring& file_path, unsigned* code_page) {
  File file(file_path, FILE_READ_DATA, FILE_SHARE_READ, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN);
  Buffer<char> buffer(static_cast<size_t>(file.size()));
  unsigned size = file.read(buffer.data(), static_cast<unsigned>(buffer.size()));
  if (size >= 2 && buffer.data()[0] == '\xFF' && buffer.data()[1] == '\xFE') {
    if (code_page)
      *code_page = CP_UTF16;
    return wstring(reinterpret_cast<wchar_t*>(buffer.data() + 2), (buffer.size() - 2) / 2);
  }
  else if (size >= 3 && buffer.data()[0] == '\xEF' && buffer.data()[1] == '\xBB' && buffer.data()[2] == '\xBF') {
    if (code_page)
      *code_page = CP_UTF8;
    return ansi_to_unicode(string(buffer.data() + 3, size - 3), CP_UTF8);
  }
  else {
    if (code_page)
      *code_page = CP_ACP;
    return ansi_to_unicode(string(buffer.data(), size), CP_ACP);
  }
}

wstring auto_rename(const wstring& file_path) {
  if (File::exists(file_path)) {
    unsigned n = 0;
    while (File::exists(file_path + L"." + int_to_str(n))) n++;
    return file_path + L"." + int_to_str(n);
  }
  else
    return file_path;
}
