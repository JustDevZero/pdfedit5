// vim:tabstop=4:shiftwidth=4:noexpandtab:textwidth=80

/*
 *
 * $RCSfile$
 *
 * $Log$
 * Revision 1.8  2006/05/31 22:36:58  hockm0bm
 * ModeConfigurationParser
 *         - setStream method implemented
 *         - first scratch of loadFromFile
 *
 * Revision 1.7  2006/05/31 20:10:22  hockm0bm
 * * PropertyMode enhanced and documented
 * * ModeRule, ModeMatcher, ModeConfigurationParser classes added
 *         - used by Modecontroller
 * * New Modecontroller implementation
 *         - IModeController removed
 *         - enherits from ModeRulesManager
 *         - setDefaultMode method added
 *         - loadFromFile added - not implemented yet (parser ready)
 *
 * Revision 1.6  2006/05/31 07:06:24  hockm0bm
 * doc update and prepared to be implemented
 *         - just for sync
 *
 */

#ifndef _MODECONTROLLER_H_
#define _MODECONTROLLER_H_

#include "static.h"
#include "utils/rulesmanager.h"
#include "utils/confparser.h"

//=====================================================================================

/** 
 * Enum describing property mode. 
 * 
 */
enum PropertyMode
{
	/** Unknown mode.
	 */
	mdUnknown, 
	
	/** Normal mode.
	 * Should be visible and read write. Normal property.
	 */
	mdNormal, 
	
	/** Hidden mode.
	 * Should not be visible but read write (when it is shown). Property is not
	 * very important to see (just technical detail) but when it is visible, it
	 * should be changeable.
	 */
	mdHidden, 
	
	/** Read-only mode.
	 * Should not be editable when it is visible. It doesn't make much sense to
	 * change such properties.
	 */
	mdReadOnly, 
	
	/** Advanced mode.
	 * Should not be visible and editable. It is very dangeroous to change such
	 * properties.
	 */
	mdAdvanced
};

/** Rule structure for mode.
 * This type is used for RulesManager RuleType in ModeController.
 *
 * @see RulesManager
 * @see ModeController
 */
struct ModeRule
{
	/** Complex value type.
	 */
	std::string type;

	/** Field name in complex type.
	 */
	std::string name;

	/** Comparator method.
	 * @param rule Rule to compare with.
	 *
	 * Given rule is same as given one if its type and name fields are same as
	 * these.
	 *
	 * @return true if given rule is same, false otherwise.
	 */
	bool equals(const ModeRule & rule)const
	{
		return type==rule.type && name==rule.name;
	}
};

/** Typedef to IRuleMatcher for ModeMatcher.
 */
typedef rulesmanager::IRuleMatcher<ModeRule> IModeMatcher; 

/** Implementator of IRuleMatcher for ModeRule.
 *
 * See functor operator for more precise information about mode rules
 * matching evaluation.
 */
class ModeMatcher: public IModeMatcher
{
public:
	typedef IModeMatcher::priority_t priority_t;
	
	/** Priority for weakest match.
	 */
	static const priority_t PRIO0=0;

	/** Priority for type only match.
	 */
	static const priority_t PRIO1=1;

	/** Priority for name only match.
	 */
	static const priority_t PRIO2=2;
	
	/** Priority for full match.
	 */
	static const priority_t PRIO3=3;

	/** Determines whether given rule matches given original.
	 * @param original Original rule.
	 * @param rule Rule to check whether matches.
	 * @param priority Place where to store priority of match (only if non NULL
	 * and matches).
	 *
	 * <b>Rules</b>:
	 * <ul>
	 * <li>original={"", ""} - rule allways matches with PRIO0 priority.
	 * <li>original={type, ""} - rule matches if rule.type==original.type with
	 * PRIO1 priority.
	 * <li>original={"", name} - rule matches if rule.name==original.name with
	 * PRIO2 priority.
	 * <li>original={type, name} - rule matches if original.equals(rule) with
	 * PRIO3 priority.
	 * <li> PRIO0 &lt PRIO1 &lt PRIO2 &lt PRIO3
	 * </ul>
	 *
	 * @return true if given rule matches original, false otherwise.
	 */
	virtual bool operator()(const ModeRule & original, const ModeRule & rule,  priority_t * priority)const;
};

/** Typedef to RulesManager for ModeController.
 */
typedef rulesmanager::RulesManager<ModeRule, PropertyMode> ModeRulesManager;

/** Simple decorator to IConfigurationParser which is able to provide key value
 * strings.
 *
 * This basic implementation is initialized in constructor and all parsing work
 * from input data is delagated to it. Returned result is just converted to
 * ModeRule and PropertyMode.
 */
class ModeConfigurationParser: public configuration::IConfigurationParser<ModeRule, PropertyMode>
{
public:
	typedef configuration::IConfigurationParser<std::string, std::string> StringParser;
private:
	/** Low level string parser.
	 * This parser is initialized in constructor and it is used for input data
	 * parsing.
	 */
	StringParser & baseParser;
public:
	/** Initialization constructor.
	 * Initializes baseParser field and configure it to have correct blanksSet,
	 * commentsSet and deliminerSet.
	 */
	ModeConfigurationParser(StringParser & parser):baseParser(parser)
	{}

	/** Sets new data stream.
	 * @param str Stream to set.
	 *
	 * Delegates to baseStream (sets stream to low level string parser).
	 *
	 * @return Current stream reference.
	 */
	std::istream * setStream(std::istream * str)
	{
		return baseParser.setStream(str);
	}
	
	/** Skips current key, value pair.
	 * Delegates to baseParser.skip()
	 */
	void skip()
	{
		baseParser.skip();
	}

	/** Checks whether we are on end of data.
	 *
	 * Delegates to baseParser.eod()
	 *
	 * @return true if there is nothing more to read, false otherwise.
	 */
	bool eod()
	{
		return baseParser.eod();
	}

	/** Parses key, value pair and transforms them to rule and mode.
	 * @param rule Reference where to put parsed rule.
	 * @param mode Reference where to put parsed mode.
	 *
	 * Delegates to baseParser.parse and transforms returned strings to  rule
	 * and mode.
	 * <p>
	 * Rule string has format:
	 * <pre>
	 * type[.name]
	 * </pre>
	 * If `.' is not found name or there are no characters behind, name is
	 * empty. Everythign before `.' is Type. It can also be empty. Both parts
	 * are trimed before set (all leading and trailing blanks re skiped).
	 * <br>
	 * Name string should be same as PropertyMode names. If not recognized,
	 * returns with an error.
	 */
	bool parse(ModeRule & rule, PropertyMode & mode);
};

/**
 * Basic mode controller implementation. 
 *
 * Provides mapping from type, name pair to Property mode, where type stands for
 * complex type name (Value of complex - dictionary - Type field value) and name
 * for field name in given complex type.
 * <p>
 * <b>Rules</b>
 * <br>
 * We will write type, name pair in following format:
 * <pre>
 * type[.name]: mode
 * </pre>
 * not specified is represented by empty string or word containing just
 * blanks.
 * <ul>
 * <li>not specified type and name. Each type name combination matches. 
 * <pre>
 * Example:
 * .: mdUnknown
 * is same as
 * : mdUnknown
 *
 * Default mode is mdUnknown
 * </pre>
 *
 * <li>type with name unspecified is same as pure type and specifies all fields
 * from dictionary with given Type field value.
 * <pre>
 * Example:
 * Page. : mdAdvanced
 * is same as
 * Page : mdAdvanced
 *
 * All fields from Page complex value is mdAdvanced
 *
 * <li>unspecified type and specified name stands for all fields with given name
 * with no respect to complex type. It has higher priority than pure type 
 * specification. 
 * <pre>
 * Example:
 * Page. : mdNormal
 * .Value: mdReadOnly
 *
 * Page.foo is mdNormal
 * Page.Value is mdReadOnly
 * Foo.Value is mdReadOnly
 * </pre>
 *
 * <li>Type, Name specifies concrete field on complex type (Type field value of
 * complex) and field name in this complex. This has the highest priority.
 * <pre>
 * Example:
 * Page. : mdNormal
 * .Value: mdReadOnly
 * Page.Value: mdAdvanced
 *
 * Page.Value is mdAdvanced
 * Page.foo is mdNormal
 * foo.Value is mdReadOnly
 * </pre>
 * 
 * <li>Most specific mapping is always used if multiple rules matches.
 * <li>No matching rule stands for default policy (value returned by
 * getDefaultMode).
 * <li>type, name are case sensitive.
 * </ul>
 *
 */
class ModeController: protected ModeRulesManager
{
	/** Default mode.
	 */
	PropertyMode defaultMode;

	/** Matcher for rules.
	 */
	ModeMatcher matcher;
public:

	typedef ModeConfigurationParser ConfParser;
	
	/** Constructor.
	 *
	 * Intiailizes defaultMode to mdUnknown.
	 * Sets ModeMatcher instance to ModeRulesManager.
	 */
	ModeController():defaultMode(mdUnknown)
	{ 
		// initializes specialized rules matcher for property modes.
		setRuleMatcher(&matcher);
	};

	/** Constructor with default mode.
	 * @param defMod Default mode to be used.
	 *
	 * Initializes defaultMode with given one.
	 * Sets ModeMatcher instance to ModeRulesManager.
	 */
	ModeController(PropertyMode defMod):defaultMode(defMod)
	{
		// initializes specialized rules matcher for property modes.
		setRuleMatcher(&matcher);
	};

	/** Virtual destructor.
	 * This method is empty, because no special treatment is required.
	 */
	virtual ~ModeController () {};

	/** Reads given configuration file.
	 * @param confFile Configuration file name.
	 * @param parser Parser to be used for file parsing.
	 *
	 * Uses given parser to get key (ModeRule) and value (PropertyMode).
	 * Those are registered using addRule inherited method.
	 */
	int loadFromFile(std::string confFile, ConfParser & parser);

	/** 
	 * Get default mode.
	 * 
	 * Default policy for non matching rules.
	 * @return defaultMode field value.
	 */
	virtual PropertyMode getDefaultMode() const
	{
		return defaultMode;
	}

	/** Sets default mode.
	 * @param mode New default mode.
	 *
	 * @return Previous value of default mode.
	 */
	virtual PropertyMode setDefaultMode(PropertyMode mode)
	{
		PropertyMode old=defaultMode;

		defaultMode=mode;

		return old;
	}

	/**
	 * Get mode for value with given type and name.
	 * @param type Type field value of complex type.
	 * @param name Name if the field in complex type.
	 * 
	 * Given parameters may be empty what means not specified.
	 * <br>
	 * If no rule matches given pair, getDefaultMode is returned.
	 * 
	 * @return Mode which is most specific for given type, name pair.
	 */
	virtual PropertyMode getMode (const std::string& type, const std::string& name) const
	{
		ModeRule rule={type, name};
		PropertyMode mode;

		// delegates to ModeRulesManager and uses returned mode
		if(findMatching(rule, &mode))
			return mode;

		// didn't match - defaultMode is used
		return defaultMode;
	}
};

#endif  //_MODECONTROLLER_H_
