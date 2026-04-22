#include <bits/stdc++.h>
using namespace std;

static bool find_number_near_keywords(const string &s, long long &out) {
    string lower = s;
    for (char &c : lower) c = tolower((unsigned char)c);
    vector<string> keys = {
        "return", "ret", "ans", "answer", "result", "expected", "output", "retval", "res", "expect"
    };
    auto parse_first_in = [&](const string &sub, long long &val) -> bool {
        // search hex like 0x... first and decimal; choose earliest occurrence
        size_t best_pos = string::npos; long long best_val = 0; bool found = false;
        // hex
        for (size_t i = 0; i + 2 <= sub.size(); ++i) {
            if (sub[i] == '0' && (sub[i+1] == 'x' || sub[i+1] == 'X')) {
                size_t j = i + 2; if (j >= sub.size()) continue;
                size_t k = j;
                while (k < sub.size() && isxdigit((unsigned char)sub[k])) ++k;
                if (k > j) {
                    string token = sub.substr(i, k - i);
                    long long v = 0;
                    try {
                        v = stoll(token, nullptr, 16);
                    } catch (...) { continue; }
                    if (!found || i < best_pos) { best_pos = i; best_val = v; found = true; }
                }
            }
        }
        // signed decimal
        for (size_t i = 0; i < sub.size(); ++i) {
            if ((sub[i] == '+' || sub[i] == '-') || isdigit((unsigned char)sub[i])) {
                size_t j = i;
                if (sub[j] == '+' || sub[j] == '-') {
                    ++j;
                    if (j >= sub.size() || !isdigit((unsigned char)sub[j])) continue;
                }
                size_t k = j;
                while (k < sub.size() && isdigit((unsigned char)sub[k])) ++k;
                string token = sub.substr(i, k - i);
                if (!token.empty() && (isdigit((unsigned char)token.back()))) {
                    long long v = 0;
                    try {
                        v = stoll(token);
                    } catch (...) { continue; }
                    if (!found || i < best_pos) { best_pos = i; best_val = v; found = true; }
                }
                i = k;
            }
        }
        if (found) { val = best_val; return true; }
        return false;
    };

    for (const string &k : keys) {
        size_t pos = 0;
        while (true) {
            pos = lower.find(k, pos);
            if (pos == string::npos) break;
            size_t start = pos; // look ahead window
            size_t end = min(lower.size(), pos + 200); // search within 200 chars after keyword
            string window = s.substr(start, end - start);
            long long val;
            if (parse_first_in(window, val)) { out = val; return true; }
            pos = pos + 1;
        }
    }
    return false;
}

static bool find_fallback_number(const string &s, long long &out) {
    // fallback: pick the last reasonable-sized integer token in the entire text
    long long last = LLONG_MAX; bool has = false;
    for (size_t i = 0; i < s.size(); ++i) {
        if ((s[i] == '+' || s[i] == '-') || isdigit((unsigned char)s[i])) {
            size_t j = i;
            if (s[j] == '+' || s[j] == '-') {
                ++j;
                if (j >= s.size() || !isdigit((unsigned char)s[j])) continue;
            }
            size_t k = j;
            while (k < s.size() && isdigit((unsigned char)s[k])) ++k;
            string token = s.substr(i, k - i);
            try {
                long long v = stoll(token);
                // filter absurd large (keep within +/-1e12)
                if (v > -(1LL<<50) && v < (1LL<<50)) { last = v; has = true; }
            } catch (...) {}
            i = k;
        }
    }
    if (has) { out = last; return true; }
    return false;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Read entire stdin (binary-safe)
    std::ostringstream oss;
    oss << cin.rdbuf();
    string data = oss.str();

    long long val = 0;
    bool ok = false;
    if (!data.empty()) {
        ok = find_number_near_keywords(data, val);
        if (!ok) ok = find_fallback_number(data, val);
    }

    if (!ok) val = 0;
    cout << val << '\n';
    return 0;
}
