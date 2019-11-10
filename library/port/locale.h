/** 
 *  locale.h
 *
 *  Copyright (C) 2002-2020 zosatapo
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __OPENCODE_LOCALE_H__
#define __OPENCODE_LOCALE_H__

#include "port/libdefs.h"
#include "port/lifetime.h"
#include "port/mutex.h"

__OPENCODE_BEGIN_NAMESPACE

/**
 *
 * A <code>Locale</code> object represents a specific geographical, political,
 * or cultural region. An operation that requires a <code>Locale</code> to perform
 * its task is called <em>locale-sensitive</em> and uses the <code>Locale</code>
 * to tailor information for the user. For example, displaying a number
 * is a locale-sensitive operation--the number should be formatted
 * according to the customs/conventions of the user's native country,
 * region, or culture.
 *
 * <P>
 * Create a <code>Locale</code> object using the constructors in this class:
 * <blockquote>
 * <pre>
 * Locale(const string& language)
 * Locale(const string& language, const string& country)
 * Locale(const string& language, const string& country, const string& variant)
 * </pre>
 * </blockquote>
 * The language argument is a valid <STRONG>ISO Language Code.</STRONG> 
 * These codes are the lower-case, two-letter codes as defined by ISO-639.
 * You can find a full list of these codes at a number of sites, such as:
 * <BR><a href ="http://www.ics.uci.edu/pub/ietf/http/related/iso639.txt">
 * <code>http://www.ics.uci.edu/pub/ietf/http/related/iso639.txt</code></a>
 *
 * <P>
 * The country argument is a valid <STRONG>ISO Country Code.</STRONG> These 
 * codes are the upper-case, two-letter codes as defined by ISO-3166.
 * You can find a full list of these codes at a number of sites, such as:
 * <BR><a href="http://www.chemie.fu-berlin.de/diverse/doc/ISO_3166.html">
 * <code>http://www.chemie.fu-berlin.de/diverse/doc/ISO_3166.html</code></a>
 *
 * <P>
 * The variant argument is a vendor or browser-specific code.
 * For example, use WIN for Windows, MAC for Macintosh, and POSIX for POSIX.
 * Where there are two variants, separate them with an underscore, and
 * put the most important one first. For example, a Traditional Spanish collation 
 * might construct a locale with parameters for language, country and variant as: 
 * "es", "ES", "Traditional_WIN".
 *
 * <P>
 * Because a <code>Locale</code> object is just an identifier for a region,
 * no validity check is performed when you construct a <code>Locale</code>.
 * If you want to see whether particular resources are available for the
 * <code>Locale</code> you construct, you must query those resources. For
 * example, ask the <code>NumberFormat</code> for the locales it supports
 * using its <code>getAvailableLocales</code> method.
 * <BR><STRONG>Note:</STRONG> When you ask for a resource for a particular
 * locale, you get back the best available match, not necessarily
 * precisely what you asked for. For more information, look at
 * {@link ResourceBundle}.
 *
 * <P>
 * The <code>Locale</code> class provides a number of convenient constants
 * that you can use to create <code>Locale</code> objects for commonly used
 * locales. For example, the following creates a <code>Locale</code> object
 * for the United States:
 * <blockquote>
 * <pre>
 * Locale.US
 * </pre>
 * </blockquote>
 *
 * <P>
 * Once you've created a <code>Locale</code> you can query it for information about
 * itself. Use <code>getCountry</code> to get the ISO Country Code and
 * <code>getLanguage</code> to get the ISO Language Code. You can
 * use <code>getDisplayCountry</code> to get the
 * name of the country suitable for displaying to the user. Similarly,
 * you can use <code>getDisplayLanguage</code> to get the name of
 * the language suitable for displaying to the user. Interestingly,
 * the <code>getDisplayXXX</code> methods are themselves locale-sensitive
 * and have two versions: one that uses the default locale and one
 * that uses the locale specified as an argument.
 *
 */

class LocaleDoImpl;

class __OPENCODE_CORE_API_DECL Locale
{
public:
  ~Locale();

  /**
   * Construct a locale from language, country, variant.
   * NOTE:  ISO 639 is not a stable standard; some of the language codes it defines
   * (specifically iw, ji, and in) have changed.  This constructor accepts both the
   * old codes (iw, ji, and in) and the new codes (he, yi, and id), but all rv
   * API on Locale will return only the OLD codes.
   * @param language lowercase two-letter ISO-639 code.
   * @param country uppercase two-letter ISO-3166 code.
   * @param variant vendor and browser specific code. See class description.
   */
  Locale(const string& language, const string& country="", const string& variant="");

  Locale();
  Locale(const Locale& rhs);
  Locale&  operator=(const Locale& rv);
  
  bool operator==(const Locale& rv) const;
  bool operator!=(const Locale& rv) const;  
  bool operator<(const Locale& rv) const;
  bool operator>(const Locale& rv) const;
  bool operator<=(const Locale& rv) const;
  bool operator>=(const Locale& rv) const;
        
public:
  /**
   * Gets the current value of the default locale for this instance
   * of the Operation System.
   * <p>
   * The Operation System sets the default locale during startup
   * based on the host environment. It is used by many locale-sensitive
   * methods if no locale is explicitly specified.
   * It can be changed using the
   * {@link #setDefault(java.base.Locale) setDefault} method.
   *
   * @return the default locale for this instance of the Operation System
   */
  static const Locale& getDefault();
  
  /**
   * Sets the default locale for this instance of the Operation System.
   * This does not affect the host locale.
   * <p>
   * If there is a security manager, its <code>checkPermission</code>
   * method is called with a <code>PropertyPermission("user.language", "write")</code>
   * permission before the default locale is changed.
   * <p>
   * The Operation System sets the default locale during startup
   * based on the host environment. It is used by many locale-sensitive
   * methods if no locale is explicitly specified.
   * <p>
   * Since changing the default locale may affect many different areas
   * of functionality, this method should only be used if the caller
   * is prepared to reinitialize locale-sensitive code running
   * within the same Operation System, such as the user interface.
   *
   * @throws SecurityException
   *        if a security manager exists and its
   *        <code>checkPermission</code> method doesn't allow the operation.
   * @throws NullPointerException if <code>newLocale</code> is null
   * @param newLocale the new default locale
   */
  static  void setDefault(const Locale& newLocale);
  
  /**
   * Returns the language code for this locale, which will either be the empty string
   * or a lowercase ISO 639 code.
   * <p>NOTE:  ISO 639 is not a stable standard-- some languages' codes have changed.
   * Locale's constructor recognizes both the new and the old codes for the languages
   * whose codes have changed, but this function always returns the old code.  If you
   * want to check for a specific language whose code has changed, don't do <pre>
   * if (locale.getLanguage().equals("he")
   *    ...
   * </pre>Instead, do<pre>
   * if (locale.getLanguage().equals(new Locale("he", "", "").getLanguage())
   *    ...</pre>
   */
  const string& getLanguage() const;
  
  /**
   * Returns the country/region code for this locale, which will either be the empty string
   * or an upercase ISO 3166 2-letter code.
   */
  const string& getCountry() const;
  
  /**
   * Returns the variant code for this locale.
   */
  const string& getVariant() const;

  /**
   * Returns a name for the locale's language that is appropriate for display to the
   * user.
   * If possible, the name returned will be localized for the default locale.
   * For example, if the locale is fr_FR and the default locale
   * is en_US, getDisplayLanguage() will return "French"; if the locale is en_US and
   * the default locale is fr_FR, getDisplayLanguage() will return "anglais".
   * If the name returned cannot be localized for the default locale,
   * (say, we don't have a Japanese name for Croatian),
   * this function falls back on the English name, and uses the ISO code as a last-resort
   * value.  If the locale doesn't specify a language, this function returns the empty string.
   */
  string getDisplayLanguage() const;

  /**
   * Returns a name for the locale's language that is appropriate for display to the
   * user.
   * If possible, the name returned will be localized according to inLocale.
   * For example, if the locale is fr_FR and inLocale
   * is en_US, getDisplayLanguage() will return "French"; if the locale is en_US and
   * inLocale is fr_FR, getDisplayLanguage() will return "anglais".
   * If the name returned cannot be localized according to inLocale,
   * (say, we don't have a Japanese name for Croatian),
   * this function falls back on the default locale, on the English name, and finally
   * on the ISO code as a last-resort value.  If the locale doesn't specify a language,
   * this function returns the empty string.
   */
  string getDisplayLanguage(const Locale& inLocale) const;

  /**
   * Returns a name for the locale's country that is appropriate for display to the
   * user.
   * If possible, the name returned will be localized for the default locale.
   * For example, if the locale is fr_FR and the default locale
   * is en_US, getDisplayCountry() will return "France"; if the locale is en_US and
   * the default locale is fr_FR, getDisplayLanguage() will return "Etats-Unis".
   * If the name returned cannot be localized for the default locale,
   * (say, we don't have a Japanese name for Croatia),
   * this function falls back on the English name, and uses the ISO code as a last-resort
   * value.  If the locale doesn't specify a country, this function returns the empty string.
   */
  string getDisplayCountry() const;

  /**
   * Returns a name for the locale's country that is appropriate for display to the
   * user.
   * If possible, the name returned will be localized according to inLocale.
   * For example, if the locale is fr_FR and inLocale
   * is en_US, getDisplayCountry() will return "France"; if the locale is en_US and
   * inLocale is fr_FR, getDisplayLanguage() will return "Etats-Unis".
   * If the name returned cannot be localized according to inLocale.
   * (say, we don't have a Japanese name for Croatia),
   * this function falls back on the default locale, on the English name, and finally
   * on the ISO code as a last-resort value.  If the locale doesn't specify a country,
   * this function returns the empty string.
   */
  string getDisplayCountry(const Locale& inLocale) const;

  /**
   * Returns a name for the locale's variant code that is appropriate for display to the
   * user.  If possible, the name will be localized for the default locale.  If the locale
   * doesn't specify a variant code, this function returns the empty string.
   */
  string getDisplayVariant() const;

  /**
   * Returns a name for the locale's variant code that is appropriate for display to the
   * user.  If possible, the name will be localized for inLocale.  If the locale
   * doesn't specify a variant code, this function returns the empty string.
   */
  string getDisplayVariant(const Locale& inLocale) const;


  /**
   * Returns a name for the locale that is appropriate for display to the
   * user.  This will be the values returned by getDisplayLanguage(), getDisplayCountry(),
   * and getDisplayVariant() assembled into a single string.  The display name will have
   * one of the following forms:<p><blockquote>
   * language (country, variant)<p>
   * language (country)<p>
   * language (variant)<p>
   * country (variant)<p>
   * language<p>
   * country<p>
   * variant<p></blockquote>
   * depending on which fields are specified in the locale.  If the language, country,
   * and variant fields are all empty, this function returns the empty string.
   */
  string getDisplayName() const;


  /**
   * Returns a name for the locale that is appropriate for display to the
   * user.  This will be the values returned by getDisplayLanguage(), getDisplayCountry(),
   * and getDisplayVariant() assembled into a single string.  The display name will have
   * one of the following forms:<p><blockquote>
   * language (country, variant)<p>
   * language (country)<p>
   * language (variant)<p>
   * country (variant)<p>
   * language<p>
   * country<p>
   * variant<p></blockquote>
   * depending on which fields are specified in the locale.  If the language, country,
   * and variant fields are all empty, this function returns the empty string.
   */
  string getDisplayName(const Locale& inLocale) const;
  
public:
  /**
   * Getter for the programmatic name of the entire locale,
   * with the language, country and variant separated by underbars.
   * Language is always lower case, and country is always upper case.
   * If the language is missing, the string will begin with an underbar.
   * If both the language and country fields are missing, this function
   * will return the empty string, even if the variant field is filled in
   * (you can't have a locale with just a variant-- the variant must accompany
   * a valid language or country code).
   * Examples: "en", "de_DE", "_GB", "en_US_WIN", "de__POSIX", "fr__MAC"
   */
  string  toString() const;


private:
  string language;
  string country;  
  string variant;
};

class __OPENCODE_CORE_API_DECL LocaleDoImpl
{
  DECLARE_LIFETIME_BEGIN(LocaleDoImpl)
  DECLARE_LIFETIME_END(LocaleDoImpl)
  
public:
  LocaleDoImpl();
  ~LocaleDoImpl();
  
public:
  const Locale& getDefault();
  void  setDefault(const Locale& newLocale);

private:
  Locale* defaultLocale;        
  Mutex   locale_mutex;
};

typedef SingletonHolder<LocaleDoImpl,CreateUsingNew<LocaleDoImpl>,SingletonWithLongevity<LocaleDoImpl>,MultiThread<LocaleDoImpl> > LocaleImpl;
__OPENCODE_INLINE unsigned int getLongevity(LocaleDoImpl* impl){return __BASE_LONGEVITY_LOCALE;}

__OPENCODE_END_NAMESPACE
#endif
