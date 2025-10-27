const API_BASE = 'http://localhost:8080/api';

function loadRecipes(filterParams = '') {
    const container = document.getElementById('recipesContainer');
    if (!container) return;

    container.innerHTML = '<div class="loading">Loading recipes...</div>';

    fetch(`${API_BASE}/recipes${filterParams}`)
        .then(response => response.json())
        .then(recipes => {
            if (recipes.length === 0) {
                container.innerHTML = '<div class="loading">No recipes found matching your criteria.</div>';
                return;
            }

            container.innerHTML = '';
            recipes.forEach(recipe => {
                const card = createRecipeCard(recipe);
                container.appendChild(card);
            });
        })
        .catch(error => {
            console.error('Error loading recipes:', error);
            container.innerHTML = '<div class="error">Failed to load recipes. Make sure the server is running.</div>';
        });
}

function createRecipeCard(recipe) {
    const card = document.createElement('div');
    card.className = 'recipe-card';

    const tags = [];
    if (recipe.is_vegan) tags.push('<span class="tag">Vegan</span>');
    if (recipe.is_vegetarian) tags.push('<span class="tag">Vegetarian</span>');
    if (recipe.is_gluten_free) tags.push('<span class="tag">Gluten-Free</span>');

    card.innerHTML = `
        <img src="${recipe.image_url || 'https://via.placeholder.com/350x200?text=No+Image'}"
             alt="${recipe.title}" class="recipe-image"
             onerror="this.src='https://via.placeholder.com/350x200?text=No+Image'">
        <div class="recipe-content">
            <h3 class="recipe-title">${recipe.title}</h3>
            <p class="recipe-description">${recipe.description}</p>
            <div class="recipe-meta">
                <span>🍖 Protein: ${recipe.protein}g</span>
                <span>🍞 Carbs: ${recipe.carbs}g</span>
                <span>⏱️ ${recipe.cook_time} min</span>
                <span>📊 ${recipe.difficulty}</span>
            </div>
            <div class="recipe-tags">
                ${tags.join('')}
            </div>
            <div class="recipe-actions">
                <button class="btn-primary btn-small" onclick="viewRecipe(${recipe.id})">View Details</button>
                <button class="btn-secondary btn-small" onclick="deleteRecipe(${recipe.id})">Delete</button>
            </div>
        </div>
    `;

    return card;
}

function applyFilters() {
    const minProtein = document.getElementById('minProtein').value;
    const maxProtein = document.getElementById('maxProtein').value;
    const minCarbs = document.getElementById('minCarbs').value;
    const maxCarbs = document.getElementById('maxCarbs').value;
    const vegan = document.getElementById('vegan').checked;
    const vegetarian = document.getElementById('vegetarian').checked;
    const glutenFree = document.getElementById('glutenFree').checked;
    const sortBy = document.getElementById('sortBy').value;
    const sortOrder = document.getElementById('sortOrder').value;

    const params = new URLSearchParams();

    if (minProtein) params.append('minProtein', minProtein);
    if (maxProtein) params.append('maxProtein', maxProtein);
    if (minCarbs) params.append('minCarbs', minCarbs);
    if (maxCarbs) params.append('maxCarbs', maxCarbs);
    if (vegan) params.append('vegan', 'true');
    if (vegetarian) params.append('vegetarian', 'true');
    if (glutenFree) params.append('glutenFree', 'true');
    if (sortBy) {
        params.append('sortBy', sortBy);
        params.append('order', sortOrder);
    }

    const queryString = params.toString();
    loadRecipes(queryString ? '?' + queryString : '');
}

function clearFilters() {
    document.getElementById('minProtein').value = '';
    document.getElementById('maxProtein').value = '';
    document.getElementById('minCarbs').value = '';
    document.getElementById('maxCarbs').value = '';
    document.getElementById('vegan').checked = false;
    document.getElementById('vegetarian').checked = false;
    document.getElementById('glutenFree').checked = false;
    document.getElementById('sortBy').value = '';
    document.getElementById('sortOrder').value = 'asc';

    loadRecipes();
}

function viewRecipe(id) {
    window.location.href = `recipe-detail.html?id=${id}`;
}

function deleteRecipe(id) {
    if (!confirm('Are you sure you want to delete this recipe?')) {
        return;
    }

    fetch(`${API_BASE}/recipes/${id}`, {
        method: 'DELETE'
    })
        .then(response => response.json())
        .then(data => {
            alert('Recipe deleted successfully!');
            loadRecipes();
        })
        .catch(error => {
            console.error('Error deleting recipe:', error);
            alert('Failed to delete recipe. Please try again.');
        });
}

function loadRecipeDetails() {
    const urlParams = new URLSearchParams(window.location.search);
    const id = urlParams.get('id');

    if (!id) {
        window.location.href = 'index.html';
        return;
    }

    const container = document.getElementById('recipeDetails');
    if (!container) return;

    container.innerHTML = '<div class="loading">Loading recipe details...</div>';

    fetch(`${API_BASE}/recipes/${id}`)
        .then(response => response.json())
        .then(recipe => {
            const tags = [];
            if (recipe.is_vegan) tags.push('<span class="tag">Vegan</span>');
            if (recipe.is_vegetarian) tags.push('<span class="tag">Vegetarian</span>');
            if (recipe.is_gluten_free) tags.push('<span class="tag">Gluten-Free</span>');

            container.innerHTML = `
                <h2>${recipe.title}</h2>
                <img src="${recipe.image_url || 'https://via.placeholder.com/800x400?text=No+Image'}"
                     alt="${recipe.title}"
                     onerror="this.src='https://via.placeholder.com/800x400?text=No+Image'">
                <div class="recipe-meta">
                    <span>🍖 Protein: ${recipe.protein}g</span>
                    <span>🍞 Carbs: ${recipe.carbs}g</span>
                    <span>⏱️ Cook Time: ${recipe.cook_time} minutes</span>
                    <span>📊 Difficulty: ${recipe.difficulty}</span>
                </div>
                <div class="recipe-tags">
                    ${tags.join('')}
                </div>
                <p>${recipe.description}</p>

                <h2>Ingredients</h2>
                <div class="ingredients-list">
                    ${recipe.ingredients.split(',').map(i => `<div>• ${i.trim()}</div>`).join('')}
                </div>

                <h2>Instructions</h2>
                <div class="instructions-list">
                    ${recipe.instructions.split('\n').map((step, idx) =>
                        `<div>${step.trim()}</div>`
                    ).join('')}
                </div>

                <div class="recipe-actions" style="margin-top: 30px;">
                    <button class="btn-secondary" onclick="window.location.href='index.html'">Back to Recipes</button>
                    <button class="btn-primary" onclick="deleteRecipe(${recipe.id})">Delete Recipe</button>
                </div>
            `;
        })
        .catch(error => {
            console.error('Error loading recipe:', error);
            container.innerHTML = '<div class="error">Failed to load recipe details.</div>';
        });
}

function initAddRecipeForm() {
    const form = document.getElementById('recipeForm');
    if (!form) return;

    form.addEventListener('submit', function(e) {
        e.preventDefault();

        const formData = new FormData();
        formData.append('title', document.getElementById('title').value);
        formData.append('description', document.getElementById('description').value);

        const imageFile = document.getElementById('image').files[0];
        if (imageFile) {
            if (imageFile.size > 5 * 1024 * 1024) {
                alert('Image file is too large. Maximum size is 5MB.');
                return;
            }
            formData.append('image', imageFile);
        }

        formData.append('protein', document.getElementById('protein').value);
        formData.append('carbs', document.getElementById('carbs').value);
        formData.append('cook_time', document.getElementById('cook_time').value);
        formData.append('difficulty', document.getElementById('difficulty').value);
        formData.append('is_vegan', document.getElementById('is_vegan').checked ? '1' : '0');
        formData.append('is_vegetarian', document.getElementById('is_vegetarian').checked ? '1' : '0');
        formData.append('is_gluten_free', document.getElementById('is_gluten_free').checked ? '1' : '0');
        formData.append('ingredients', document.getElementById('ingredients').value);
        formData.append('instructions', document.getElementById('instructions').value);

        fetch(`${API_BASE}/recipes`, {
            method: 'POST',
            body: formData
        })
            .then(response => response.json())
            .then(data => {
                alert('Recipe added successfully!');
                window.location.href = 'index.html';
            })
            .catch(error => {
                console.error('Error adding recipe:', error);
                alert('Failed to add recipe. Please try again.');
            });
    });
}

if (document.getElementById('recipesContainer')) {
    loadRecipes();
}

if (document.getElementById('recipeDetails')) {
    loadRecipeDetails();
}

if (document.getElementById('recipeForm')) {
    initAddRecipeForm();
}
