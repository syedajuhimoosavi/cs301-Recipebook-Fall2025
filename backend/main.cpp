#include "httplib.h"
#include "database.h"
#include "recipe.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>

std::string recipeToJson(const Recipe& recipe) {
    std::stringstream ss;
    ss << "{";
    ss << "\"id\":" << recipe.id << ",";
    ss << "\"title\":\"" << recipe.title << "\",";
    ss << "\"description\":\"" << recipe.description << "\",";
    ss << "\"image_url\":\"" << recipe.image_url << "\",";
    ss << "\"protein\":" << recipe.protein << ",";
    ss << "\"carbs\":" << recipe.carbs << ",";
    ss << "\"is_vegan\":" << (recipe.is_vegan ? "true" : "false") << ",";
    ss << "\"is_vegetarian\":" << (recipe.is_vegetarian ? "true" : "false") << ",";
    ss << "\"is_gluten_free\":" << (recipe.is_gluten_free ? "true" : "false") << ",";
    ss << "\"cook_time\":" << recipe.cook_time << ",";
    ss << "\"difficulty\":\"" << recipe.difficulty << "\",";
    ss << "\"ingredients\":\"" << recipe.ingredients << "\",";
    ss << "\"instructions\":\"" << recipe.instructions << "\",";
    ss << "\"created_at\":\"" << recipe.created_at << "\"";
    ss << "}";
    return ss.str();
}

std::string recipesToJson(const std::vector<Recipe>& recipes) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < recipes.size(); ++i) {
        ss << recipeToJson(recipes[i]);
        if (i < recipes.size() - 1) ss << ",";
    }
    ss << "]";
    return ss.str();
}

std::string getQueryParam(const httplib::Request& req, const std::string& key, const std::string& defaultValue = "") {
    if (req.has_param(key)) {
        return req.get_param_value(key);
    }
    return defaultValue;
}

double getQueryParamDouble(const httplib::Request& req, const std::string& key, double defaultValue = -1.0) {
    if (req.has_param(key)) {
        try {
            return std::stod(req.get_param_value(key));
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

bool getQueryParamBool(const httplib::Request& req, const std::string& key) {
    if (req.has_param(key)) {
        std::string value = req.get_param_value(key);
        return value == "true" || value == "1";
    }
    return false;
}

int main() {
    Database db("recipes.db");
    if (!db.initialize()) {
        std::cerr << "Failed to initialize database" << std::endl;
        return 1;
    }

    httplib::Server svr;

    svr.set_mount_point("/", "../frontend");
    svr.set_mount_point("/uploads", "../uploads");

    svr.Get("/api/recipes", [&](const httplib::Request& req, httplib::Response& res) {
        std::vector<Recipe> recipes;

        bool hasFilters = req.has_param("minProtein") || req.has_param("maxProtein") ||
                          req.has_param("minCarbs") || req.has_param("maxCarbs") ||
                          req.has_param("vegan") || req.has_param("vegetarian") ||
                          req.has_param("glutenFree");

        bool hasSorting = req.has_param("sortBy");

        if (hasFilters) {
            double minProtein = getQueryParamDouble(req, "minProtein");
            double maxProtein = getQueryParamDouble(req, "maxProtein");
            double minCarbs = getQueryParamDouble(req, "minCarbs");
            double maxCarbs = getQueryParamDouble(req, "maxCarbs");
            bool vegan = getQueryParamBool(req, "vegan");
            bool vegetarian = getQueryParamBool(req, "vegetarian");
            bool glutenFree = getQueryParamBool(req, "glutenFree");

            recipes = db.filterRecipes(minProtein, maxProtein, minCarbs, maxCarbs,
                                       vegan, vegetarian, glutenFree);
        } else if (hasSorting) {
            std::string sortBy = getQueryParam(req, "sortBy", "created_at");
            std::string order = getQueryParam(req, "order", "desc");
            recipes = db.sortRecipes(sortBy, order);
        } else {
            recipes = db.getAllRecipes();
        }

        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(recipesToJson(recipes), "application/json");
    });

    svr.Get("/api/recipes/:id", [&](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.path_params.at("id"));
        Recipe recipe = db.getRecipeById(id);

        res.set_header("Access-Control-Allow-Origin", "*");

        if (recipe.id == -1) {
            res.status = 404;
            res.set_content("{\"error\":\"Recipe not found\"}", "application/json");
        } else {
            res.set_content(recipeToJson(recipe), "application/json");
        }
    });

    svr.Post("/api/recipes", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");

        Recipe recipe;
        recipe.title = req.form.get_field("title");
        recipe.description = req.form.get_field("description");

        if (req.form.has_file("image")) {
            auto file = req.form.get_file("image");
            std::string filename = std::to_string(std::time(nullptr)) + "_" + file.filename;
            std::string filepath = "../uploads/" + filename;

            std::ofstream ofs(filepath, std::ios::binary);
            ofs << file.content;
            ofs.close();

            recipe.image_url = "/uploads/" + filename;
        } else {
            recipe.image_url = "";
        }

        recipe.protein = std::stod(req.form.get_field("protein"));
        recipe.carbs = std::stod(req.form.get_field("carbs"));
        recipe.is_vegan = req.form.get_field("is_vegan") == "1";
        recipe.is_vegetarian = req.form.get_field("is_vegetarian") == "1";
        recipe.is_gluten_free = req.form.get_field("is_gluten_free") == "1";
        recipe.cook_time = std::stoi(req.form.get_field("cook_time"));
        recipe.difficulty = req.form.get_field("difficulty");
        recipe.ingredients = req.form.get_field("ingredients");
        recipe.instructions = req.form.get_field("instructions");

        if (db.addRecipe(recipe)) {
            res.status = 201;
            res.set_content("{\"message\":\"Recipe created successfully\"}", "application/json");
        } else {
            res.status = 500;
            res.set_content("{\"error\":\"Failed to create recipe\"}", "application/json");
        }
    });

    svr.Put("/api/recipes/:id", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");

        int id = std::stoi(req.path_params.at("id"));

        Recipe recipe;
        recipe.title = req.form.get_field("title");
        recipe.description = req.form.get_field("description");

        if (req.form.has_file("image")) {
            auto file = req.form.get_file("image");
            std::string filename = std::to_string(std::time(nullptr)) + "_" + file.filename;
            std::string filepath = "../uploads/" + filename;

            std::ofstream ofs(filepath, std::ios::binary);
            ofs << file.content;
            ofs.close();

            recipe.image_url = "/uploads/" + filename;
        } else {
            Recipe existing = db.getRecipeById(id);
            recipe.image_url = existing.image_url;
        }

        recipe.protein = std::stod(req.form.get_field("protein"));
        recipe.carbs = std::stod(req.form.get_field("carbs"));
        recipe.is_vegan = req.form.get_field("is_vegan") == "1";
        recipe.is_vegetarian = req.form.get_field("is_vegetarian") == "1";
        recipe.is_gluten_free = req.form.get_field("is_gluten_free") == "1";
        recipe.cook_time = std::stoi(req.form.get_field("cook_time"));
        recipe.difficulty = req.form.get_field("difficulty");
        recipe.ingredients = req.form.get_field("ingredients");
        recipe.instructions = req.form.get_field("instructions");

        if (db.updateRecipe(id, recipe)) {
            res.set_content("{\"message\":\"Recipe updated successfully\"}", "application/json");
        } else {
            res.status = 500;
            res.set_content("{\"error\":\"Failed to update recipe\"}", "application/json");
        }
    });

    svr.Delete("/api/recipes/:id", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");

        int id = std::stoi(req.path_params.at("id"));

        if (db.deleteRecipe(id)) {
            res.set_content("{\"message\":\"Recipe deleted successfully\"}", "application/json");
        } else {
            res.status = 500;
            res.set_content("{\"error\":\"Failed to delete recipe\"}", "application/json");
        }
    });

    svr.Options("/api/recipes", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    });

    svr.Options("/api/recipes/:id", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    });

    std::cout << "Server starting on http://localhost:8080" << std::endl;
    std::cout << "API endpoints available at http://localhost:8080/api/recipes" << std::endl;

    svr.listen("0.0.0.0", 8080);

    return 0;
}
