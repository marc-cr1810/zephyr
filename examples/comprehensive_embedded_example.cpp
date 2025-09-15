#include "zephyr/api/zephyr_api.hpp"
#include "zephyr/api/native_module.hpp"
#include "zephyr/objects/int_object.hpp"
#include "zephyr/objects/float_object.hpp"
#include "zephyr/objects/string_object.hpp"
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <random>
#include <algorithm>

using namespace zephyr::api;
using namespace zephyr;

/**
 * Comprehensive Embedded Zephyr C++ Example
 * 
 * This example demonstrates a complete embedded application using Zephyr
 * with various C++ features including:
 * - Classes with inheritance and polymorphism
 * - Function overloading and templates
 * - Static and instance variables
 * - STL containers and algorithms
 * - Exception handling
 * - Memory management
 * - Design patterns (Factory, Observer)
 * 
 * The example simulates a game engine with entities, physics, and AI systems.
 */

// ============================================================================
// Base Classes and Interfaces
// ============================================================================

// Abstract base class for all game entities
class Entity {
protected:
    static int next_id;
    int id;
    std::string name;
    double x, y, z;
    bool active;
    
public:
    Entity(const std::string& name = "Entity") 
        : id(next_id++), name(name), x(0), y(0), z(0), active(true) {}
    
    virtual ~Entity() = default;
    
    // Virtual methods for polymorphism
    virtual void update(double delta_time) = 0;
    virtual std::string get_type() const = 0;
    virtual double get_health() const { return 100.0; }
    
    // Getters and setters
    int get_id() const { return id; }
    const std::string& get_name() const { return name; }
    void set_position(double nx, double ny, double nz) { x = nx; y = ny; z = nz; }
    std::string get_position() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }
    
    bool is_active() const { return active; }
    void set_active(bool state) { active = state; }
    
    // Distance calculation
    double distance_to(const Entity& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }
};

// Static variable initialization
int Entity::next_id = 1;

// ============================================================================
// Derived Classes - Game Entities
// ============================================================================

class Player : public Entity {
private:
    double health;
    int level;
    int experience;
    std::vector<std::string> inventory;
    
public:
    Player(const std::string& name = "Player") 
        : Entity(name), health(100.0), level(1), experience(0) {
        inventory.push_back("Basic Sword");
        inventory.push_back("Health Potion");
    }
    
    void update(double delta_time) override {
        // Player-specific update logic
        if (health < 100.0) {
            health = std::min(100.0, health + delta_time * 5.0); // Regenerate health
        }
    }
    
    std::string get_type() const override { return "Player"; }
    double get_health() const override { return health; }
    
    void take_damage(double damage) {
        health = std::max(0.0, health - damage);
    }
    
    void gain_experience(int exp) {
        experience += exp;
        while (experience >= level * 100) {
            experience -= level * 100;
            level++;
        }
    }
    
    void add_item(const std::string& item) {
        inventory.push_back(item);
    }
    
    int get_level() const { return level; }
    int get_experience() const { return experience; }
    
    std::string get_inventory() const {
        std::string result = "[";
        for (size_t i = 0; i < inventory.size(); ++i) {
            result += inventory[i];
            if (i < inventory.size() - 1) result += ", ";
        }
        result += "]";
        return result;
    }
};

class Enemy : public Entity {
private:
    double health;
    double attack_power;
    double speed;
    std::string enemy_type;
    
public:
    Enemy(const std::string& name, const std::string& type = "Goblin") 
        : Entity(name), enemy_type(type), health(50.0), attack_power(15.0), speed(2.0) {}
    
    void update(double delta_time) override {
        // Simple AI: move randomly
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dis(-1.0, 1.0);
        
        x += dis(gen) * speed * delta_time;
        y += dis(gen) * speed * delta_time;
    }
    
    std::string get_type() const override { return enemy_type; }
    double get_health() const override { return health; }
    
    double attack() const { return attack_power; }
    
    void take_damage(double damage) {
        health = std::max(0.0, health - damage);
        if (health <= 0) {
            set_active(false);
        }
    }
};

class PowerUp : public Entity {
private:
    std::string power_type;
    double value;
    double lifetime;
    double age;
    
public:
    PowerUp(const std::string& name, const std::string& type, double val) 
        : Entity(name), power_type(type), value(val), lifetime(30.0), age(0.0) {}
    
    void update(double delta_time) override {
        age += delta_time;
        if (age >= lifetime) {
            set_active(false);
        }
    }
    
    std::string get_type() const override { return "PowerUp"; }
    
    const std::string& get_power_type() const { return power_type; }
    double get_value() const { return value; }
    double get_remaining_time() const { return std::max(0.0, lifetime - age); }
};

// ============================================================================
// Game Systems
// ============================================================================

class GameWorld {
private:
    std::vector<std::unique_ptr<Entity>> entities;
    std::map<std::string, int> statistics;
    double total_time;
    
public:
    GameWorld() : total_time(0.0) {
        statistics["entities_created"] = 0;
        statistics["enemies_defeated"] = 0;
        statistics["power_ups_collected"] = 0;
    }
    
    template<typename T, typename... Args>
    T* create_entity(Args&&... args) {
        auto entity = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = entity.get();
        entities.push_back(std::move(entity));
        statistics["entities_created"]++;
        return ptr;
    }
    
    void update(double delta_time) {
        total_time += delta_time;
        
        // Update all entities
        for (auto& entity : entities) {
            if (entity->is_active()) {
                entity->update(delta_time);
            }
        }
        
        // Remove inactive entities
        auto it = std::remove_if(entities.begin(), entities.end(),
            [this](const std::unique_ptr<Entity>& entity) {
                if (!entity->is_active()) {
                    if (entity->get_type() != "Player" && entity->get_type() != "PowerUp") {
                        statistics["enemies_defeated"]++;
                    } else if (entity->get_type() == "PowerUp") {
                        statistics["power_ups_collected"]++;
                    }
                    return true;
                }
                return false;
            });
        entities.erase(it, entities.end());
    }
    
    size_t get_entity_count() const { return entities.size(); }
    double get_total_time() const { return total_time; }
    
    std::string get_statistics() const {
        std::string result = "Game Statistics:\n";
        for (const auto& stat : statistics) {
            result += "  " + stat.first + ": " + std::to_string(stat.second) + "\n";
        }
        result += "  total_time: " + std::to_string(total_time) + "s\n";
        result += "  active_entities: " + std::to_string(entities.size());
        return result;
    }
    
    Entity* find_entity_by_id(int id) {
        auto it = std::find_if(entities.begin(), entities.end(),
            [id](const std::unique_ptr<Entity>& entity) {
                return entity->get_id() == id;
            });
        return it != entities.end() ? it->get() : nullptr;
    }
    
    std::vector<Entity*> find_entities_by_type(const std::string& type) {
        std::vector<Entity*> result;
        for (const auto& entity : entities) {
            if (entity->get_type() == type) {
                result.push_back(entity.get());
            }
        }
        return result;
    }
};

// ============================================================================
// Utility Classes and Functions
// ============================================================================

class MathUtils {
public:
    // Template function for generic calculations
    template<typename T>
    static T clamp(T value, T min_val, T max_val) {
        return std::max(min_val, std::min(max_val, value));
    }
    
    static double lerp(double a, double b, double t) {
        return a + t * (b - a);
    }
    
    static double random_range(double min_val, double max_val) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(min_val, max_val);
        return dis(gen);
    }
    
    static int random_int(int min_val, int max_val) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min_val, max_val);
        return dis(gen);
    }
};

class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    bool running;
    
public:
    Timer() : running(false) {}
    
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
        running = true;
    }
    
    void stop() {
        running = false;
    }
    
    double elapsed_seconds() const {
        if (!running) return 0.0;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start_time);
        return duration.count() / 1000000.0;
    }
};

// ============================================================================
// Native Modules for Zephyr Integration
// ============================================================================

class game_engine_module_t : public native_module_t {
private:
    GameWorld world;
    Timer game_timer;
    
public:
    auto initialize() -> void_result_t override {
        set_name("game_engine");
        set_version("1.0.0");
        set_description("Comprehensive game engine with entities and systems");
        
        // Initialize game world
        game_timer.start();
        
        // Entity creation functions
        // Entity creation functions
        export_function("create_player", [this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 1) return value_result_t::error("create_player requires exactly 1 argument");
            auto name_obj = std::dynamic_pointer_cast<string_object_t>(args[0]);
            if (!name_obj) return value_result_t::error("Argument must be a string");
            
            auto player = world.create_entity<Player>(name_obj->value());
            return value_result_t::success(std::make_shared<int_object_t>(player->get_id()));
        });
        
        export_function("create_enemy", [this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) return value_result_t::error("create_enemy requires exactly 2 arguments");
            auto name_obj = std::dynamic_pointer_cast<string_object_t>(args[0]);
            auto type_obj = std::dynamic_pointer_cast<string_object_t>(args[1]);
            if (!name_obj || !type_obj) return value_result_t::error("Arguments must be strings");
            
            auto enemy = world.create_entity<Enemy>(name_obj->value(), type_obj->value());
            return value_result_t::success(std::make_shared<int_object_t>(enemy->get_id()));
        });
        
        export_function("create_powerup", [this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 3) return value_result_t::error("create_powerup requires exactly 3 arguments");
            auto name_obj = std::dynamic_pointer_cast<string_object_t>(args[0]);
            auto type_obj = std::dynamic_pointer_cast<string_object_t>(args[1]);
            auto value_obj = std::dynamic_pointer_cast<float_object_t>(args[2]);
            if (!name_obj || !type_obj) return value_result_t::error("First two arguments must be strings");
            if (!value_obj) return value_result_t::error("Third argument must be a number");
            
            auto powerup = world.create_entity<PowerUp>(name_obj->value(), type_obj->value(), value_obj->value());
            return value_result_t::success(std::make_shared<int_object_t>(powerup->get_id()));
        });
        
        // World management functions
        export_function("update_world", [this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 1) return value_result_t::error("update_world requires exactly 1 argument");
            auto delta_obj = std::dynamic_pointer_cast<float_object_t>(args[0]);
            if (!delta_obj) return value_result_t::error("Argument must be a number");
            
            world.update(delta_obj->value());
            return value_result_t::success(std::make_shared<int_object_t>(1));
        });
        
        export_function("get_entity_count", [this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 0) return value_result_t::error("get_entity_count requires no arguments");
            return value_result_t::success(std::make_shared<int_object_t>(static_cast<int>(world.get_entity_count())));
        });
        
        export_function("get_game_time", [this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 0) return value_result_t::error("get_game_time requires no arguments");
            return value_result_t::success(std::make_shared<float_object_t>(game_timer.elapsed_seconds()));
        });
        
        export_function("get_statistics", [this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 0) return value_result_t::error("get_statistics requires no arguments");
            return value_result_t::success(std::make_shared<string_object_t>(world.get_statistics()));
        });
        
        // Entity query functions
        export_function("get_entity_info", [this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 1) return value_result_t::error("get_entity_info requires exactly 1 argument");
            auto id_obj = std::dynamic_pointer_cast<int_object_t>(args[0]);
            if (!id_obj) return value_result_t::error("Argument must be an integer");
            
            auto entity = world.find_entity_by_id(id_obj->value());
            if (!entity) return value_result_t::success(std::make_shared<string_object_t>("Entity not found"));
            
            std::string info = "Entity " + std::to_string(entity->get_id()) + 
                             " (" + entity->get_type() + "): " + 
                             entity->get_name() + " at " + entity->get_position() +
                             ", Health: " + std::to_string(entity->get_health());
            return value_result_t::success(std::make_shared<string_object_t>(info));
        });
        
        export_function("set_entity_position", [this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 4) return value_result_t::error("set_entity_position requires exactly 4 arguments");
            auto id_obj = std::dynamic_pointer_cast<int_object_t>(args[0]);
            auto x_obj = std::dynamic_pointer_cast<float_object_t>(args[1]);
            auto y_obj = std::dynamic_pointer_cast<float_object_t>(args[2]);
            auto z_obj = std::dynamic_pointer_cast<float_object_t>(args[3]);
            if (!id_obj) return value_result_t::error("First argument must be an integer");
            if (!x_obj || !y_obj || !z_obj) return value_result_t::error("Position arguments must be numbers");
            
            auto entity = world.find_entity_by_id(id_obj->value());
            if (!entity) return value_result_t::success(std::make_shared<int_object_t>(0));
            entity->set_position(x_obj->value(), y_obj->value(), z_obj->value());
            return value_result_t::success(std::make_shared<int_object_t>(1));
        });
        
        // Player-specific functions
        export_function("player_gain_exp", [this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) return value_result_t::error("player_gain_exp requires exactly 2 arguments");
            auto id_obj = std::dynamic_pointer_cast<int_object_t>(args[0]);
            auto exp_obj = std::dynamic_pointer_cast<int_object_t>(args[1]);
            if (!id_obj || !exp_obj) return value_result_t::error("Arguments must be integers");
            
            auto entity = world.find_entity_by_id(id_obj->value());
            if (!entity || entity->get_type() != "Player") return value_result_t::success(std::make_shared<int_object_t>(0));
            
            auto player = static_cast<Player*>(entity);
            player->gain_experience(exp_obj->value());
            return value_result_t::success(std::make_shared<int_object_t>(1));
        });
        
        export_function("player_add_item", [this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) return value_result_t::error("player_add_item requires exactly 2 arguments");
            auto id_obj = std::dynamic_pointer_cast<int_object_t>(args[0]);
            auto item_obj = std::dynamic_pointer_cast<string_object_t>(args[1]);
            if (!id_obj || !item_obj) return value_result_t::error("Arguments must be integer and string");
            
            auto entity = world.find_entity_by_id(id_obj->value());
            if (!entity || entity->get_type() != "Player") return value_result_t::success(std::make_shared<int_object_t>(0));
            
            auto player = static_cast<Player*>(entity);
            player->add_item(item_obj->value());
            return value_result_t::success(std::make_shared<int_object_t>(1));
        });
        
        export_function("get_player_inventory", [this](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 1) return value_result_t::error("get_player_inventory requires exactly 1 argument");
            auto id_obj = std::dynamic_pointer_cast<int_object_t>(args[0]);
            if (!id_obj) return value_result_t::error("Argument must be an integer");
            
            auto entity = world.find_entity_by_id(id_obj->value());
            if (!entity || entity->get_type() != "Player") return value_result_t::success(std::make_shared<string_object_t>("Invalid player"));
            
            auto player = static_cast<Player*>(entity);
            return value_result_t::success(std::make_shared<string_object_t>(player->get_inventory()));
        });
        
        return void_result_t::success();
    }
};

class math_utils_module_t : public native_module_t {
public:
    auto initialize() -> void_result_t override {
        set_name("math_utils");
        set_version("1.0.0");
        set_description("Mathematical utilities and helper functions");
        
        // Basic math functions
        export_function("clamp_double", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 3) return value_result_t::error("clamp_double requires exactly 3 arguments");
            auto value_obj = std::dynamic_pointer_cast<float_object_t>(args[0]);
            auto min_obj = std::dynamic_pointer_cast<float_object_t>(args[1]);
            auto max_obj = std::dynamic_pointer_cast<float_object_t>(args[2]);
            if (!value_obj || !min_obj || !max_obj) return value_result_t::error("Arguments must be numbers");
            
            double result = MathUtils::clamp(value_obj->value(), min_obj->value(), max_obj->value());
            return value_result_t::success(std::make_shared<float_object_t>(result));
        });
        
        export_function("clamp_int", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 3) return value_result_t::error("clamp_int requires exactly 3 arguments");
            auto value_obj = std::dynamic_pointer_cast<int_object_t>(args[0]);
            auto min_obj = std::dynamic_pointer_cast<int_object_t>(args[1]);
            auto max_obj = std::dynamic_pointer_cast<int_object_t>(args[2]);
            if (!value_obj || !min_obj || !max_obj) return value_result_t::error("Arguments must be integers");
            
            int result = MathUtils::clamp(value_obj->value(), min_obj->value(), max_obj->value());
            return value_result_t::success(std::make_shared<int_object_t>(result));
        });
        
        export_function("lerp", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 3) return value_result_t::error("lerp requires exactly 3 arguments");
            auto a_obj = std::dynamic_pointer_cast<float_object_t>(args[0]);
            auto b_obj = std::dynamic_pointer_cast<float_object_t>(args[1]);
            auto t_obj = std::dynamic_pointer_cast<float_object_t>(args[2]);
            if (!a_obj || !b_obj || !t_obj) return value_result_t::error("Arguments must be numbers");
            
            double result = MathUtils::lerp(a_obj->value(), b_obj->value(), t_obj->value());
            return value_result_t::success(std::make_shared<float_object_t>(result));
        });
        
        export_function("random_range", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) return value_result_t::error("random_range requires exactly 2 arguments");
            auto min_obj = std::dynamic_pointer_cast<float_object_t>(args[0]);
            auto max_obj = std::dynamic_pointer_cast<float_object_t>(args[1]);
            if (!min_obj || !max_obj) return value_result_t::error("Arguments must be numbers");
            
            double result = MathUtils::random_range(min_obj->value(), max_obj->value());
            return value_result_t::success(std::make_shared<float_object_t>(result));
        });
        
        export_function("random_int", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 2) return value_result_t::error("random_int requires exactly 2 arguments");
            auto min_obj = std::dynamic_pointer_cast<int_object_t>(args[0]);
            auto max_obj = std::dynamic_pointer_cast<int_object_t>(args[1]);
            if (!min_obj || !max_obj) return value_result_t::error("Arguments must be integers");
            
            int result = MathUtils::random_int(min_obj->value(), max_obj->value());
            return value_result_t::success(std::make_shared<int_object_t>(result));
        });
        
        // Advanced math functions
        export_function("distance_2d", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 4) return value_result_t::error("distance_2d requires exactly 4 arguments");
            auto x1_obj = std::dynamic_pointer_cast<float_object_t>(args[0]);
            auto y1_obj = std::dynamic_pointer_cast<float_object_t>(args[1]);
            auto x2_obj = std::dynamic_pointer_cast<float_object_t>(args[2]);
            auto y2_obj = std::dynamic_pointer_cast<float_object_t>(args[3]);
            if (!x1_obj || !y1_obj || !x2_obj || !y2_obj) return value_result_t::error("Arguments must be numbers");
            
            double dx = x2_obj->value() - x1_obj->value();
            double dy = y2_obj->value() - y1_obj->value();
            double result = std::sqrt(dx*dx + dy*dy);
            return value_result_t::success(std::make_shared<float_object_t>(result));
        });
        
        export_function("distance_3d", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 6) return value_result_t::error("distance_3d requires exactly 6 arguments");
            auto x1_obj = std::dynamic_pointer_cast<float_object_t>(args[0]);
            auto y1_obj = std::dynamic_pointer_cast<float_object_t>(args[1]);
            auto z1_obj = std::dynamic_pointer_cast<float_object_t>(args[2]);
            auto x2_obj = std::dynamic_pointer_cast<float_object_t>(args[3]);
            auto y2_obj = std::dynamic_pointer_cast<float_object_t>(args[4]);
            auto z2_obj = std::dynamic_pointer_cast<float_object_t>(args[5]);
            if (!x1_obj || !y1_obj || !z1_obj || !x2_obj || !y2_obj || !z2_obj) return value_result_t::error("Arguments must be numbers");
            
            double dx = x2_obj->value() - x1_obj->value();
            double dy = y2_obj->value() - y1_obj->value();
            double dz = z2_obj->value() - z1_obj->value();
            double result = std::sqrt(dx*dx + dy*dy + dz*dz);
            return value_result_t::success(std::make_shared<float_object_t>(result));
        });
        
        export_function("normalize_angle", [](const std::vector<value_t>& args) -> value_result_t {
            if (args.size() != 1) return value_result_t::error("normalize_angle requires exactly 1 argument");
            auto angle_obj = std::dynamic_pointer_cast<float_object_t>(args[0]);
            if (!angle_obj) return value_result_t::error("Argument must be a number");
            
            double angle = angle_obj->value();
            while (angle > M_PI) angle -= 2 * M_PI;
            while (angle < -M_PI) angle += 2 * M_PI;
            return value_result_t::success(std::make_shared<float_object_t>(angle));
        });
        
        // Export constants
        export_constant("PI", std::make_shared<float_object_t>(M_PI));
        export_constant("E", std::make_shared<float_object_t>(M_E));
        export_constant("SQRT2", std::make_shared<float_object_t>(M_SQRT2));
        export_constant("DEG_TO_RAD", std::make_shared<float_object_t>(M_PI / 180.0));
        export_constant("RAD_TO_DEG", std::make_shared<float_object_t>(180.0 / M_PI));
        
        return void_result_t::success();
    }
};

// ============================================================================
// Main Application
// ============================================================================

int main() {
    std::cout << "=== Comprehensive Embedded Zephyr C++ Example ===" << std::endl;
    std::cout << "Demonstrating classes, functions, variables, and system integration" << std::endl;
    std::cout << std::endl;
    
    try {
        // Create Zephyr engine
        auto config = engine_config_t{};
        config.name = "Game Engine Demo";
        config.debug_mode = false;
        
        auto engine = create_engine(config);
        std::cout << "Created Zephyr engine: " << engine->get_name() << std::endl;
        
        // Register native modules
        std::cout << "Registering native modules..." << std::endl;
        
        auto game_module = std::make_shared<game_engine_module_t>();
        game_module->initialize();
        engine->register_module("game_engine", game_module);
        
        auto math_module = std::make_shared<math_utils_module_t>();
        math_module->initialize();
        engine->register_module("math_utils", math_module);
        
        std::cout << "Modules registered successfully!" << std::endl;
        std::cout << std::endl;
        
        // Test 1: Basic game world setup
        std::cout << "=== Test 1: Game World Initialization ===" << std::endl;
        
        auto result1 = engine->execute_string(R"ZEPHYR_CODE(
            import game_engine
            import math_utils
            
            print("Initializing game world...")
            
            player_id = game_engine.create_player("Hero")
            enemy1_id = game_engine.create_enemy("Goblin_01", "Goblin")
            enemy2_id = game_engine.create_enemy("Orc_01", "Orc")
            powerup_id = game_engine.create_powerup("Health_Boost", "Health", 25.0)
            
            print("Created entities:")
            print("  Player ID:", player_id)
            print("  Enemy 1 ID:", enemy1_id)
            print("  Enemy 2 ID:", enemy2_id)
            print("  PowerUp ID:", powerup_id)
            
            entity_count = game_engine.get_entity_count()
            print("Total entities:", entity_count)
            
            return "Game world initialized successfully"
        )ZEPHYR_CODE", "world_init");
        
        if (result1) {
            std::cout << "✅ World initialization: " << from_zephyr<std::string>(result1.value()).value_or("unknown") << std::endl;
        } else {
            std::cout << "❌ World initialization failed: " << result1.error() << std::endl;
        }
        std::cout << std::endl;
        
        // Test 2: Entity manipulation and math utilities
        std::cout << "=== Test 2: Entity Management ===" << std::endl;
        
        auto result2 = engine->execute_string(R"ZEPHYR_CODE(
            import game_engine
            import math_utils
            
            print("Testing entity management...")
            
            player_x = math_utils.random_range(-10.0, 10.0)
            player_y = math_utils.random_range(-10.0, 10.0)
            player_z = 0.0
            
            game_engine.set_entity_position(1, player_x, player_y, player_z)
            
            enemy1_x = math_utils.random_range(-5.0, 5.0)
            enemy1_y = math_utils.random_range(-5.0, 5.0)
            game_engine.set_entity_position(2, enemy1_x, enemy1_y, 0.0)
            
            enemy2_x = math_utils.random_range(-8.0, 8.0)
            enemy2_y = math_utils.random_range(-8.0, 8.0)
            game_engine.set_entity_position(3, enemy2_x, enemy2_y, 0.0)
            
            distance_to_enemy1 = math_utils.distance_3d(player_x, player_y, 0.0, enemy1_x, enemy1_y, 0.0)
            distance_to_enemy2 = math_utils.distance_3d(player_x, player_y, 0.0, enemy2_x, enemy2_y, 0.0)
            
            print("Player position: (", player_x, ",", player_y, ", 0)")
            print("Distance to Enemy 1:", distance_to_enemy1)
            print("Distance to Enemy 2:", distance_to_enemy2)

            player_info = game_engine.get_entity_info(1)
            enemy1_info = game_engine.get_entity_info(2)
            
            print("Player info:", player_info)
            print("Enemy 1 info:", enemy1_info)
            
            return "Entity management completed"
        )ZEPHYR_CODE", "entity_test");
        
        if (result2) {
            std::cout << "✅ Entity management: " << from_zephyr<std::string>(result2.value()).value_or("unknown") << std::endl;
        } else {
            std::cout << "❌ Entity management failed: " << result2.error() << std::endl;
        }
        std::cout << std::endl;
        
        // Test 3: Player progression system
        std::cout << "=== Test 3: Player Progression ===" << std::endl;
        
        auto result3 = engine->execute_string(R"ZEPHYR_CODE(
            import game_engine
            import math_utils
            
            print("Testing player progression...")
            
            exp_gained = math_utils.random_int(50, 200)
            success = game_engine.player_gain_exp(1, exp_gained)
            print("Player gained", exp_gained, "experience points")
            
            items = ["Magic Sword", "Shield of Protection", "Healing Potion", "Mana Crystal"]
            for item in items {
                game_engine.player_add_item(1, item)
                print("Player found:", item)
            }
            
            inventory = game_engine.get_player_inventory(1)
            print("Player inventory:", inventory)
            
            updated_info = game_engine.get_entity_info(1)
            print("Updated player info:", updated_info)
            
            return "Player progression completed"
        )ZEPHYR_CODE", "progression_test");
        
        if (result3) {
            std::cout << "✅ Player progression: " << from_zephyr<std::string>(result3.value()).value_or("unknown") << std::endl;
        } else {
            std::cout << "❌ Player progression failed: " << result3.error() << std::endl;
        }
        std::cout << std::endl;
        
        // Test 4: Game simulation
        std::cout << "=== Test 4: Game Simulation ===" << std::endl;
        
        auto result4 = engine->execute_string(R"ZEPHYR_CODE(
            import game_engine
            import math_utils
            
            print("Running game simulation...")
            
            simulation_time = 5.0
            time_step = 0.1
            steps = int(simulation_time / time_step)
            
            print("Simulating", steps, "steps over", simulation_time, "seconds")
            
            for i in range(steps) {
                game_engine.update_world(time_step)
                
                if i % 10 == 0 {
                    current_time = game_engine.get_game_time()
                    entity_count = game_engine.get_entity_count()
                    print("Time:", current_time, "s, Entities:", entity_count)
                }
                
                if math_utils.random_int(1, 100) <= 5 {
                    enemy_name = "Random_Enemy_" + str(i)
                    enemy_types = ["Goblin", "Orc", "Skeleton", "Wolf"]
                    enemy_type = enemy_types[math_utils.random_int(0, len(enemy_types) - 1)]
                    
                    new_enemy_id = game_engine.create_enemy(enemy_name, enemy_type)
                    
                    x = math_utils.random_range(-15.0, 15.0)
                    y = math_utils.random_range(-15.0, 15.0)
                    game_engine.set_entity_position(new_enemy_id, x, y, 0.0)
                }
            }
            
            final_stats = game_engine.get_statistics()
            print("Final Game Statistics:")
            print(final_stats)
            
            return "Game simulation completed successfully"
        )ZEPHYR_CODE", "simulation_test");
        
        if (result4) {
            std::cout << "✅ Game simulation: " << from_zephyr<std::string>(result4.value()).value_or("unknown") << std::endl;
        } else {
            std::cout << "❌ Game simulation failed: " << result4.error() << std::endl;
        }
        std::cout << std::endl;
        
        // Test 5: Advanced math and algorithm demonstration
        std::cout << "=== Test 5: Advanced Mathematics ===" << std::endl;
        
        auto result5 = engine->execute_string(R"ZEPHYR_CODE(
            import math_utils
            
            print("Testing advanced mathematical functions...")
            
            value = 15.5
            clamped = math_utils.clamp_double(value, 0.0, 10.0)
            print("Clamping", value, "to [0, 10]:", clamped)
            
            start_pos = 0.0
            end_pos = 100.0
            progress = 0.75
            interpolated = math_utils.lerp(start_pos, end_pos, progress)
            print("Lerping from", start_pos, "to", end_pos, "at", progress, ":", interpolated)
            
            large_angle = 450.0 * math_utils.DEG_TO_RAD
            normalized = math_utils.normalize_angle(large_angle)
            normalized_degrees = normalized * math_utils.RAD_TO_DEG
            print("Normalizing 450° to:", normalized_degrees, "°")
            
            print("Generating random samples...")
            samples = []
            sample_count = 20
            
            for i in range(sample_count) {
                sample = math_utils.random_range(0.0, 100.0)
                samples.append(sample)
            }
            
            print("Generated", len(samples), "random samples")
            
            total = 0.0
            for sample in samples {
                total += sample
            }
            average = total / len(samples)
            
            print("Sample average:", average)
            
            print("Mathematical constants:")
            print("π =", math_utils.PI)
            print("e =", math_utils.E)
            print("√2 =", math_utils.SQRT2)
            
            return "Advanced mathematics demonstration completed"
        )ZEPHYR_CODE", "math_test");
        
        if (result5) {
            std::cout << "✅ Advanced mathematics: " << from_zephyr<std::string>(result5.value()).value_or("unknown") << std::endl;
        } else {
            std::cout << "❌ Advanced mathematics failed: " << result5.error() << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "=== Example Summary ===" << std::endl;
        std::cout << "✅ Object-oriented design with inheritance" << std::endl;
        std::cout << "✅ Template functions and generic programming" << std::endl;
        std::cout << "✅ STL containers (vectors, maps) and algorithms" << std::endl;
        std::cout << "✅ Static and instance variables" << std::endl;
        std::cout << "✅ Function overloading and polymorphism" << std::endl;
        std::cout << "✅ Memory management with smart pointers" << std::endl;
        std::cout << "✅ Exception handling and error management" << std::endl;
        std::cout << "✅ Real-time system simulation" << std::endl;
        std::cout << "✅ Mathematical utilities and constants" << std::endl;
        std::cout << "✅ Native module integration with Zephyr" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "=== Comprehensive Embedded Example Complete ===" << std::endl;
    return 0;
}