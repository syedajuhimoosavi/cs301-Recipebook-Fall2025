#ifndef RECIPE_H
#define RECIPE_H

#include <string>

struct Recipe {
    int id;
    std::string title;
    std::string description;
    std::string image_url;
    double protein;
    double carbs;
    bool is_vegan;
    bool is_vegetarian;
    bool is_gluten_free;
    int cook_time;
    std::string difficulty;
    std::string ingredients;
    std::string instructions;
    std::string created_at;
};

#endif
