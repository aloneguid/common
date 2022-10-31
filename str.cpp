#include <string>
#include <vector>
#include <windows.h>
#include <cmath>
#include <sstream>
#include <regex>
#include <algorithm>
#include "str.h"

using namespace std;

namespace str {
    std::wstring to_wstr(const std::string& str) {
        if (str.empty()) return std::wstring();
        int size_needed = ::MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
        std::wstring wstrTo(size_needed, 0);
        ::MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }

    std::string to_str(const std::wstring& wstr) {
        if (wstr.empty()) return std::string();
        int size_needed = ::WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
        std::string strTo(size_needed, 0);
        ::WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
        return strTo;
    }

    int to_int(const std::string& str) {
        return atoi(str.c_str());
    }

    // trim from start (in place)
    void ltrim(std::string& s, const string& chars) {
        while (s.size() > 0) {
            size_t idx = chars.find(s[0]);
            if (idx == string::npos) return;
            s.erase(0, 1);
        }
    }

    // trim from end (in place)
    void rtrim(std::string& s, const string& chars) {
        while (s.size() > 0) {
            size_t idx = chars.find(*(s.rbegin()));
            if (idx == string::npos) return;
            s.erase(s.size() - 1, 1);
        }
    }

    void trim(std::string& s, const string& chars) {
        ltrim(s, chars);
        rtrim(s, chars);
    }

    void upper(std::string& s) {
        transform(s.begin(), s.end(), s.begin(), ::toupper);
    }

    void lower(std::string& s) {
        transform(s.begin(), s.end(), s.begin(), ::tolower);
    }

    void unescape_special_chars(std::string& s) {
        replace_all(s, "\\r", "\r");
        replace_all(s, "\\t", "\t");
        replace_all(s, "\\b", "\b");
        replace_all(s, "\\n", "\n");
        replace_all(s, "\\f", "\f");
    }

    /// <summary>
    /// Replaces (in-place) all occurances of target with replacement. Taken from : http://stackoverflow.com/questions/3418231/c-replace-part-of-a-string-with-another-string.
    /// </summary>
    /// <param name="str"></param>
    /// <param name="target"></param>
    /// <param name="replecament"></param>
    /// <returns></returns>
    template<class T>
    bool replace_all(std::basic_string<T>& str, const std::basic_string<T>& target, const std::basic_string<T>& replacement) {
        if (target.empty()) {
            return false;
        }

        size_t start_pos = 0;
        const bool found_substring = str.find(target, start_pos) != std::string::npos;

        while ((start_pos = str.find(target, start_pos)) != std::string::npos) {
            str.replace(start_pos, target.length(), replacement);
            start_pos += replacement.length();
        }

        return found_substring;
    }

    std::vector<std::string> split(const std::string& str, const std::string& delimiter, bool trim_lines) {
        std::vector<std::string> strings;

        std::string::size_type pos = 0;
        std::string::size_type prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos) {
            string el = str.substr(prev, pos - prev);
            if (trim_lines) trim(el);
            strings.push_back(el);
            prev = pos + 1;
        }

        // To get the last substring (or only, if delimiter is not found)
        strings.push_back(prev >= str.size() ? "" : str.substr(prev));

        return strings;
    }

    std::string to_human_readable_size(unsigned long size) {
        int i{};
        double mantissa = size;
        for (; mantissa >= 1024.; mantissa /= 1024., ++i) {}

        //mantissa = std::ceil(mantissa * 10.) / 10.;
        //return std::to_string(mantissa) + "BKMGTPE"[i];

        ostringstream out;
        out.precision(2);
        out << std::fixed << mantissa;
        return out.str() + "BKMGTPE"[i];
    }

    std::vector<std::string> match_all_regex(const std::string& expression, const std::string& input) {
        regex r(expression);
        vector<string> result;
        string source = input;

        smatch sm;
        while (regex_search(source, sm, r)) {
            result.push_back(sm.str());

            source = sm.suffix();
        }

        return result;
    }

    std::string get_domain_from_url(const std::string& url) {
        regex r("https?:\\/\\/(www\\.)?([-a-zA-Z0-9@:%._\\+~#=]{1,256})");
        smatch sm;
        // group 2 is the domain
        if (regex_search(url, sm, r) && sm.size() == 3) {
            return sm[2].str();
        }

        return "";
    }

    void replace_all(std::string& s, const std::string& search, const std::string& replacement) {
        std::string::size_type n = 0;
        while ((n = s.find(search, n)) != std::string::npos) {
            s.replace(n, search.size(), replacement);
            n += replacement.size();
        }
    }

    std::string humanise(int value, string singular, string plural, string once, string twice) {
        if (value == 1 && !once.empty()) {
            return once;
        }

        if (value == 2 && !twice.empty()) {
            return twice;
        }

        string r = std::to_string(value);
        bool is_singular = r.ends_with('1');
        r += "";

        r += singular;
        if (!is_singular) {
            r += "s";
        }
        return r;
    }

    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    bool is_base64(unsigned char c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; (i < 4); i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];

            while ((i++ < 3))
                ret += '=';

        }

        return ret;

    }

    std::string base64_decode(std::string const& encoded_string) {
        int in_len = encoded_string.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::string ret;

        while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
            char_array_4[i++] = encoded_string[in_]; in_++;
            if (i == 4) {
                for (i = 0; i < 4; i++)
                    char_array_4[i] = base64_chars.find(char_array_4[i]);

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++)
                    ret += char_array_3[i];
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 4; j++)
                char_array_4[j] = 0;

            for (j = 0; j < 4; j++)
                char_array_4[j] = base64_chars.find(char_array_4[j]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
        }

        return ret;
    }

    size_t word_count(const std::string& sentence) {
        size_t r = 0;

        size_t wl = 0;
        for (char ch : sentence) {
            bool is_word_char = iscsym(ch);

            if (is_word_char) {
                wl += 1;
            } else {
                if (wl > 0) {
                    r += 1;
                    wl = 0;
                }
            }
        }

        if (wl > 0) r += 1;

        return r;
    }

    std::string remove_non_ascii(const std::string& s) {
        string r;
        for (unsigned char ch : s) {
            if (ch >= 0 && ch <= 127) r += ch;
        }
        return r;
    }

    std::string strip_html(const std::string& s) {
        const regex pattern("\\<.*?\\>");
        return regex_replace(s, pattern, "");
    }

    string rgx_extract(const std::string& s, const std::string& expr) {
        string r;
        regex rgx(expr);
        smatch sm;
        string source = s;
        while (regex_search(source, sm, rgx)) {
            if (!r.empty()) r += "\n";
            r += sm.str();
            source = sm.suffix();
        }
        return r;
    }

    std::vector<std::string> rgx_extract_to_vec(const std::string& s, const std::string& expr) {
        vector<string> r;
        regex rgx(expr);
        smatch sm;
        string source = s;
        while (regex_search(source, sm, rgx)) {
            r.push_back(sm.str());
            source = sm.suffix();
        }
        return r;
    }
}