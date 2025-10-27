#ifndef DATABASE_H
#define DATABASE_H

#include "recipe.h"
#include <vector>
#include <string>
#include <sqlite3.h>

class Database {
private:
    sqlite3* db;
    std::string db_path;

public:
    Database(const std::string& path);
    ~Database();

    bool initialize();
    std::vector<Recipe> getAllRecipes();
    std::vector<Recipe> filterRecipes(double minProtein, double maxProtein,
                                      double minCarbs, double maxCarbs,
                                      bool veganOnly, bool vegetarianOnly,
                                      bool glutenFreeOnly);
    std::vector<Recipe> sortRecipes(const std::string& sortBy, const std::string& order);
    Recipe getRecipeById(int id);
    bool addRecipe(const Recipe& recipe);
    bool updateRecipe(int id, const Recipe& recipe);
    bool deleteRecipe(int id);
};

#endif
