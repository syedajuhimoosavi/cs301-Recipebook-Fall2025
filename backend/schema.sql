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

INSERT INTO recipes (title, description, image_url, protein, carbs, is_vegan, is_vegetarian, is_gluten_free, cook_time, difficulty, ingredients, instructions) VALUES
('Spinach & Feta Rolls', 'Easy to make and full of flavor, with creamy feta and fresh spinach wrapped in flaky puff pastry. Perfect for a quick snack or a simple meal.', 'Images/fetarolls.JPG', 12.5, 28.0, 0, 1, 0, 30, 'easy', 'Puff pastry, Spinach (200g), Feta cheese (150g), Olive oil, Garlic (2 cloves), Salt, Pepper', '1. Preheat oven to 200°C\n2. Sauté spinach and garlic in olive oil\n3. Mix with crumbled feta\n4. Roll puff pastry and cut into squares\n5. Add filling and fold\n6. Bake for 25-30 minutes until golden'),
('Chocolate Chip Cookies', 'A classic, comforting treat. They are soft and chewy with just the right amount of chocolate chips, making them perfect for any time you need a sweet fix.', 'Images/cookies.JPG', 4.2, 52.0, 0, 1, 0, 20, 'easy', 'Flour (2 cups), Butter (1 cup), Sugar (3/4 cup), Brown sugar (3/4 cup), Eggs (2), Vanilla extract, Chocolate chips (2 cups), Baking soda, Salt', '1. Preheat oven to 180°C\n2. Cream butter and sugars\n3. Add eggs and vanilla\n4. Mix in flour, baking soda, and salt\n5. Fold in chocolate chips\n6. Bake for 12-15 minutes'),
('Tiramisu', 'The rich and creamy delight of homemade tiramisu, where layers of espresso-soaked ladyfingers meet velvety mascarpone cheese and a hint of cocoa powder.', 'Images/tiramisu.JPG', 8.5, 35.0, 0, 1, 0, 45, 'medium', 'Ladyfinger cookies (200g), Mascarpone cheese (500g), Eggs (4), Sugar (100g), Espresso coffee (1 cup), Cocoa powder, Marsala wine (optional)', '1. Brew strong espresso and let cool\n2. Separate egg yolks and whites\n3. Beat yolks with sugar until creamy\n4. Fold in mascarpone\n5. Beat egg whites to stiff peaks and fold in\n6. Dip ladyfingers in espresso\n7. Layer in dish\n8. Refrigerate 4-6 hours\n9. Dust with cocoa before serving');
