#ifndef ENDS_WITH_H
#define ENDS_WITH_H

// http://stackoverflow.com/a/874160/383967 w/ modifications
bool ends_with (std::string const &s, std::string const &ending) {
    if (s.length() >= ending.length()) {
        return (0 == s.compare (s.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
#endif // ENDS_WITH_H
