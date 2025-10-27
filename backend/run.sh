#!/bin/bash

# Simple script to check and run the recipe server

echo "=== Recipe Server Launcher ==="
echo ""

# Check if we're in the right directory
if [ ! -f "main.cpp" ]; then
    echo "❌ Error: Not in the backend directory!"
    echo "Please run: cd backend"
    exit 1
fi

# Check if database exists
if [ ! -f "recipes.db" ]; then
    echo "⚠️  Database not found. Creating it now..."
    if ! command -v sqlite3 &> /dev/null; then
        echo "❌ Error: sqlite3 not installed!"
        echo "Install it with: brew install sqlite3"
        exit 1
    fi
    sqlite3 recipes.db < schema.sql
    echo "✅ Database created!"
    echo ""
fi

# Check if server is built
if [ ! -f "recipe_server" ]; then
    echo "⚠️  Server not built yet. Building now..."
    if ! command -v make &> /dev/null; then
        echo "❌ Error: make not installed!"
        echo "Install with: xcode-select --install"
        exit 1
    fi
    make
    if [ $? -ne 0 ]; then
        echo "❌ Build failed! Check the errors above."
        exit 1
    fi
    echo "✅ Server built successfully!"
    echo ""
fi

# Run the server
echo "🚀 Starting Recipe Server..."
echo "   Open your browser to: http://localhost:8080"
echo "   Press Ctrl+C to stop"
echo ""
./recipe_server
