// RecipeLogger.h
#if !defined(_RECIPELOGGER_H)
#define _RECIPELOGGER_H

#include "Composition.h"
#include "Table.h"
#include "InputXML.h"

#include <set>
#include <map>

#define RL RecipeLogger::Instance()

/**
   map of recipe name to composition
 */
typedef std::map<std::string,CompositionPtr> RecipeMap; 

/**
   set of decay times
 */
typedef std::set<double> decay_times;

/**
   map of composition times decayed
 */
typedef std::map<CompositionPtr,decay_times> DecayTimesMap; 

/**
   map of decay time to composition
 */
typedef std::map<double,CompositionPtr> DaughterMap; 

/**
   map of recipe composition to its decayed daughters
 */
typedef std::map<CompositionPtr,DaughterMap> DecayChainMap; 

/**
   The RecipeLogger manages the list of recipes held in memory
   during a simulation. It works in conjunction with the IsoVector
   class to efficiently manage isotopic-related memory.
 */
class RecipeLogger {
  /* --- Singleton Members and Methods --- */
 public: 
  friend class Composition;

  /**
     Gives all simulation objects global access to the RecipeLogger by 
     returning a pointer to it. 
     Like the Highlander, there can be only one. 
      
     @return a pointer to the RecipeLogger 
   */
  static RecipeLogger* Instance();

 protected:
  /**
     The (protected) constructor for this class, which can only be 
     called indirectly by the client. 
   */
  RecipeLogger();

 private:
  /**
     A pointer to this RecipeLogger once it has been initialized. 
   */
  static RecipeLogger* instance_;
  /* --- */

  /* --- Recipe Logging --- */
 public:
  /**
     loads the recipes from the input file 
   */
  static void load_recipes();

  /**
     loads a specific recipe 
   */
  static void load_recipe(xmlNodePtr cur);
  
  /**
     logs a new recipe with the simulation
     - logs recipe with BookKeeper
   */
  static void logRecipe(CompositionPtr recipe);

  /**
     logs a new named recipe with the simulation
     - adds recipe to IsoVector's static containers
     - calls the logRecipe() method
   */
  static void logRecipe(std::string name, CompositionPtr recipe);

  /**
     logs a new named recipe with the simulation
     - sets the parent of and decay time child
     - calls the logRecipe() method
     @param t_f -> total time decayed from parent to child
   */
  static void logRecipeDecay(CompositionPtr parent, CompositionPtr child, double t_f);
  
  /**
     checks if the recipe has been logged (i.e. it exists in the simulation)
   */
  static bool recipeLogged(std::string name);

  /**
     the total number of recipes 
   */
  static int recipeCount();   

  /**
     print all recipes 
   */
  static void printRecipes();

  /**
     checks if the composition is logged
   */
  static bool compositionDecayable(CompositionPtr comp);

  /**
     checks if the parent has already been decayed by this time
   */
  static bool daughterLogged(CompositionPtr parent, double time);

 private:
  /**
     adds recipe to containers tracking decayed recipes
   */
  static void storeDecayableRecipe(CompositionPtr recipe);

  /**
     accessing a recipe 
   */
  static CompositionPtr Recipe(std::string name);

  /**
     add a new decay time for a parent composition
   */
  static void addDecayTimes(CompositionPtr parent, double time);

  /**
     accessing a set of decay times 
   */
  static decay_times& decayTimes(CompositionPtr parent);

  /**
     accessing the daughters of a parent
   */
  static DaughterMap& Daughters(CompositionPtr parent);

  /**
     accessing a specific daughter 
   */
  static CompositionPtr& Daughter(CompositionPtr parent, double time);

  /**
     add a daughter to a map of daughters
   */
  static void addDaughter(CompositionPtr parent, CompositionPtr child, double time);

  /**
     calls recipeLogged() and throws an error if false
   */
  static void checkRecipe(std::string name);

  /**
     calls compositionDecayable() and throws an error if false
   */
  static void checkDecayable(CompositionPtr parent);

  /**
     calls daughterLogged() and throws an error if false
   */
  static void checkDaughter(CompositionPtr parent, double time);

  /**
     Stores the next available state ID 
   */
  static int nextStateID_;

  /**
     a container of recipes 
   */
  static RecipeMap recipes_;

  /**
     a container of recipes in each decay chain
   */
  static DecayChainMap decay_chains_;

  /**
     a container of decay times that recipes have gone through
   */
  static DecayTimesMap decay_times_;

 /* -- Output Database Interaction  -- */ 
 public:
  /**
     the isotopics output database Table 
   */
  static table_ptr iso_table;
  
  /* /\** */
  /*    return the agent table's primary key  */
  /*  *\/ */
  /* primary_key_ref pkref(); */
  
  /**
     returns true if a new state was recorded, false if already in db
  */
  void recordState();
  
 private:
  /**
     Define the database table on the first Message's init 
   */
  static void define_table();

  /**
     Add an isotopic state to the table 
   */
  static void addToTable(Composition& comp);

  /* /\** */
  /*    Store information about the transactions's primary key  */
  /*  *\/ */
  /* primary_key_ref pkref_; */
 /* -- */ 
};

#endif

