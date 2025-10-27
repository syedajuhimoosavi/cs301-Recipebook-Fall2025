#include "database.h"
#include <iostream>
#include <sstream>

Database::Database(const std::string& path) : db(nullptr), db_path(path) {}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
    }
}

bool Database::initialize() {
    int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    std::string schema = R"(
        CREATE TABLE IF NOT EXISTS recipes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            description TEXT NOT NULL,
            image_url TEXT,
            protein REAL DEFAULT 0,
            carbs REAL DEFAULT 0,
            is_vegan INTEGER DEFAULT 0,
            is_vegetarian INTEGER DEFAULT 0,
            is_gluten_free INTEGER DEFAULT 0,
            cook_time INTEGER DEFAULT 0,
            difficulty TEXT DEFAULT 'medium',
            ingredients TEXT NOT NULL,
            instructions TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";

    char* errMsg;
    rc = sqlite3_exec(db, schema.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

std::vector<Recipe> Database::getAllRecipes() {
    std::vector<Recipe> recipes;
    std::string query = "SELECT * FROM recipes ORDER BY created_at DESC";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return recipes;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Recipe recipe;
        recipe.id = sqlite3_column_int(stmt, 0);
        recipe.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        recipe.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        recipe.image_url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        recipe.protein = sqlite3_column_double(stmt, 4);
        recipe.carbs = sqlite3_column_double(stmt, 5);
        recipe.is_vegan = sqlite3_column_int(stmt, 6);
        recipe.is_vegetarian = sqlite3_column_int(stmt, 7);
        recipe.is_gluten_free = sqlite3_column_int(stmt, 8);
        recipe.cook_time = sqlite3_column_int(stmt, 9);
        recipe.difficulty = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
        recipe.ingredients = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        recipe.instructions = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
        recipe.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13));

        recipes.push_back(recipe);
    }

    sqlite3_finalize(stmt);
    return recipes;
}

std::vector<Recipe> Database::filterRecipes(double minProtein, double maxProtein,
                                             double minCarbs, double maxCarbs,
                                             bool veganOnly, bool vegetarianOnly,
                                             bool glutenFreeOnly) {
    std::vector<Recipe> recipes;
    std::stringstream query;

    query << "SELECT * FROM recipes WHERE 1=1";

    if (minProtein >= 0) query << " AND protein >= " << minProtein;
    if (maxProtein >= 0) query << " AND protein <= " << maxProtein;
    if (minCarbs >= 0) query << " AND carbs >= " << minCarbs;
    if (maxCarbs >= 0) query << " AND carbs <= " << maxCarbs;
    if (veganOnly) query << " AND is_vegan = 1";
    if (vegetarianOnly) query << " AND is_vegetarian = 1";
    if (glutenFreeOnly) query << " AND is_gluten_free = 1";

    query << " ORDER BY created_at DESC";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.str().c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return recipes;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Recipe recipe;
        recipe.id = sqlite3_column_int(stmt, 0);
        recipe.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        recipe.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        recipe.image_url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        recipe.protein = sqlite3_column_double(stmt, 4);
        recipe.carbs = sqlite3_column_double(stmt, 5);
        recipe.is_vegan = sqlite3_column_int(stmt, 6);
        recipe.is_vegetarian = sqlite3_column_int(stmt, 7);
        recipe.is_gluten_free = sqlite3_column_int(stmt, 8);
        recipe.cook_time = sqlite3_column_int(stmt, 9);
        recipe.difficulty = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
        recipe.ingredients = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        recipe.instructions = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
        recipe.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13));

        recipes.push_back(recipe);
    }

    sqlite3_finalize(stmt);
    return recipes;
}

std::vector<Recipe> Database::sortRecipes(const std::string& sortBy, const std::string& order) {
    std::vector<Recipe> recipes;
    std::stringstream query;

    query << "SELECT * FROM recipes ORDER BY ";

    if (sortBy == "cook_time") {
        query << "cook_time";
    } else if (sortBy == "difficulty") {
        query << "difficulty";
    } else {
        query << "created_at";
    }

    if (order == "asc") {
        query << " ASC";
    } else {
        query << " DESC";
    }

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.str().c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return recipes;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Recipe recipe;
        recipe.id = sqlite3_column_int(stmt, 0);
        recipe.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        recipe.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        recipe.image_url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        recipe.protein = sqlite3_column_double(stmt, 4);
        recipe.carbs = sqlite3_column_double(stmt, 5);
        recipe.is_vegan = sqlite3_column_int(stmt, 6);
        recipe.is_vegetarian = sqlite3_column_int(stmt, 7);
        recipe.is_gluten_free = sqlite3_column_int(stmt, 8);
        recipe.cook_time = sqlite3_column_int(stmt, 9);
        recipe.difficulty = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
        recipe.ingredients = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        recipe.instructions = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
        recipe.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13));

        recipes.push_back(recipe);
    }

    sqlite3_finalize(stmt);
    return recipes;
}

Recipe Database::getRecipeById(int id) {
    Recipe recipe;
    recipe.id = -1;

    std::string query = "SELECT * FROM recipes WHERE id = ?";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return recipe;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        recipe.id = sqlite3_column_int(stmt, 0);
        recipe.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        recipe.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        recipe.image_url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        recipe.protein = sqlite3_column_double(stmt, 4);
        recipe.carbs = sqlite3_column_double(stmt, 5);
        recipe.is_vegan = sqlite3_column_int(stmt, 6);
        recipe.is_vegetarian = sqlite3_column_int(stmt, 7);
        recipe.is_gluten_free = sqlite3_column_int(stmt, 8);
        recipe.cook_time = sqlite3_column_int(stmt, 9);
        recipe.difficulty = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
        recipe.ingredients = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        recipe.instructions = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
        recipe.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13));
    }

    sqlite3_finalize(stmt);
    return recipe;
}

bool Database::addRecipe(const Recipe& recipe) {
    std::string query = R"(
        INSERT INTO recipes (title, description, image_url, protein, carbs,
                            is_vegan, is_vegetarian, is_gluten_free,
                            cook_time, difficulty, ingredients, instructions)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, recipe.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, recipe.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, recipe.image_url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, recipe.protein);
    sqlite3_bind_double(stmt, 5, recipe.carbs);
    sqlite3_bind_int(stmt, 6, recipe.is_vegan ? 1 : 0);
    sqlite3_bind_int(stmt, 7, recipe.is_vegetarian ? 1 : 0);
    sqlite3_bind_int(stmt, 8, recipe.is_gluten_free ? 1 : 0);
    sqlite3_bind_int(stmt, 9, recipe.cook_time);
    sqlite3_bind_text(stmt, 10, recipe.difficulty.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, recipe.ingredients.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 12, recipe.instructions.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool Database::updateRecipe(int id, const Recipe& recipe) {
    std::string query = R"(
        UPDATE recipes SET title = ?, description = ?, image_url = ?,
                          protein = ?, carbs = ?, is_vegan = ?,
                          is_vegetarian = ?, is_gluten_free = ?,
                          cook_time = ?, difficulty = ?,
                          ingredients = ?, instructions = ?
        WHERE id = ?
    )";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, recipe.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, recipe.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, recipe.image_url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, recipe.protein);
    sqlite3_bind_double(stmt, 5, recipe.carbs);
    sqlite3_bind_int(stmt, 6, recipe.is_vegan ? 1 : 0);
    sqlite3_bind_int(stmt, 7, recipe.is_vegetarian ? 1 : 0);
    sqlite3_bind_int(stmt, 8, recipe.is_gluten_free ? 1 : 0);
    sqlite3_bind_int(stmt, 9, recipe.cook_time);
    sqlite3_bind_text(stmt, 10, recipe.difficulty.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, recipe.ingredients.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 12, recipe.instructions.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 13, id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool Database::deleteRecipe(int id) {
    std::string query = "DELETE FROM recipes WHERE id = ?";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}
