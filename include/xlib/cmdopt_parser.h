#pragma once
//#ifndef __RUNTIME_CMDOPTPARSER_H__
//#define __RUNTIME_CMDOPTPARSER_H__

/**
 * CmdOptParser.h --
 *     Simple command-line option parsing. See ctl.cpp for
 *     an example of how to use me.
 */

#ifndef __cplusplus
typedef unsigned char bool;
#define nullptr NULL
#define false   0
#define true    1
typedef struct _cmdOptDesc cmdOptDesc;
#endif

#if   CMD_OPT_ONE_LOCAL_COPY
#define CMD_OPT_STGCLS static
#elif CMD_OPT_ONE_SHARED_COPY
#define CMD_OPT_STGCLS extern
#else
#define CMD_OPT_STGCLS /* simply use default stg class */
#endif

////////////////////////////////////////////////////////////////////////////////////////
//
// We provide convenience helpers which may or may not be used; the linker will strip
// out any unused function bodies anyway, so just disable the warning in this file.
//
#pragma GCC diagnostic ignored "-Wunused-function"

////////////////////////////////////////////////////////////////////////////////////////
//
// Are we being included once in the entire project, to define the function bodies?
//
#if CMD_OPT_ONE_LOCAL_COPY || !CMD_OPT_ONE_SHARED_COPY
#define CMD_OPT_DEFS 1
#else
#define CMD_OPT_DEFS 0
#endif

////////////////////////////////////////////////////////////////////////////////////////
//
// Define the option/switch leading character based on where we are
//
#ifdef _WIN32
#define CMDSWT_CH  '/'
#define CMDSWT_STR "/"
#else
#define CMDSWT_CH  '-'
#define CMDSWT_STR "-"
#endif

////////////////////////////////////////////////////////////////////////////////////////
//
// Define a typedef for the option value validator callbacks (for convenience).
//
typedef bool (*CMD_OPT_VALCB)(const char* optBeg, const char* value);

////////////////////////////////////////////////////////////////////////////////////////
//
// These descriptors hold the info we need for each command-line option
//
struct _cmdOptDesc
{
  cmdOptDesc*   codNextOpt; // simple linked list

  const char*   codInfoStr; // option description
  const char*   codValArg;  // name of option value argument (if any)

  CMD_OPT_VALCB codValidate;// validator callback (if any)

  const char*   codLongNm;  // string name of option (i.e. the "long" name)
  unsigned char codLetter:7;// single-letter version of option
  unsigned char codQuoted:1;// are optional quotes to be stripped?

  unsigned char codType;    // 'B'=bool,'I'=int,'U'=uint,'S'=string

  union
  {
    int           codDefValInt;
    unsigned int  codDefValUint;
    bool          codDefValBool;
    const char*   codDefValStr;
  }
                codDefVal;  // default value (if any)

  union
  {
    int          *codVarPtrInt;
    unsigned int *codVarPtrUint;
    bool         *codVarPtrBool;
    const char*  *codVarPtrStr;
  }
                codVarPtr;  // address of flag value (i.e. "&FLAG_<name>")
};

/* All options are kept in a simple (and backward) linked list */
CMD_OPT_STGCLS cmdOptDesc*  cmdOptDescList;

#define __DEFINE_CMDOPT(name, letter, longnm, argname, type, tpnm, quoted, default_value, valfn, description) \
  CMD_OPT_STGCLS char       FLAG_LETTER_##name = letter;                                                      \
  CMD_OPT_STGCLS type       FLAG_##name        = (type)(default_value);                                       \
  CMD_OPT_STGCLS int        FAKE_##name        = (registerCmdOpt##tpnm(letter, longnm, description, argname, quoted, valfn, &FLAG_##name), 0);

#define CMDOPT_DEFINE_I(var_name, lett, long_name, argname,                            def, valfn, desc) \
        __DEFINE_CMDOPT(var_name, lett, long_name, argname,          int,  Int, false, def, valfn, desc);

#define CMDOPT_DEFINE_U(var_name, lett, long_name, argname,                            def, valfn, desc) \
        __DEFINE_CMDOPT(var_name, lett, long_name, argname, unsigned int, Uint, false, def, valfn, desc);

#define CMDOPT_DEFINE_S(var_name, lett, long_name, argname,                            def, valfn, desc) \
        __DEFINE_CMDOPT(var_name, lett, long_name, argname,  const char*,  Str, false, def, valfn, desc);

#define CMDOPT_DEFINE_Q(var_name, lett, long_name, argname,                            def, valfn, desc) \
        __DEFINE_CMDOPT(var_name, lett, long_name, argname,  const char*,  Str,  true, def, valfn, desc);

#define CMDOPT_DEFINE_B(var_name, lett, long_name, argname,                            def, valfn, desc) \
        __DEFINE_CMDOPT(var_name, lett, long_name, argname,         bool, Bool, false, def, valfn, desc);

CMD_OPT_STGCLS
void registerCmdOptAll(int let_name, const char* str_name, const char* info, const char* arg_name, int type, bool quote, CMD_OPT_VALCB valfn, cmdOptDesc* desc)
#if CMD_OPT_DEFS
{
  desc->codInfoStr  = info;
  desc->codValArg   = arg_name;

  desc->codLetter   = let_name;
  desc->codLongNm   = str_name;

  desc->codValidate = valfn;

  desc->codType     = type;

  desc->codNextOpt  = cmdOptDescList;
                      cmdOptDescList = desc;
}
#endif // CMD_OPT_DEFS
;

#define CMDOPT_REGFN(type, tpnm, tpdval)                                             \
                                                                                     \
  inline static void                                                                 \
  registerCmdOpt##tpnm(int           let_name,                                       \
                                   const char*   str_name,                           \
                                   const char*   info,                               \
                                   const char*   arg_name,                           \
                                   bool          quote,                              \
                                   CMD_OPT_VALCB valfn,                              \
                                   type         *var_ptr)                            \
  {                                                                                  \
    /*cmdOptDesc* nd = new cmdOptDesc;  */                                           \
    cmdOptDesc* nd   = (cmdOptDesc *) malloc(sizeof(cmdOptDesc));                                 \
                                                                                     \
    nd->codDefVal.codDefVal##tpnm = *var_ptr;                                        \
    nd->codVarPtr.codVarPtr##tpnm =  var_ptr;                                        \
                                                                                     \
    nd->codQuoted                 = quote;                                           \
                                                                                     \
    registerCmdOptAll(let_name, str_name, info, arg_name, tpdval, quote, valfn, nd); \
  }

CMDOPT_REGFN(         int,  Int, 'I');
CMDOPT_REGFN(unsigned int, Uint, 'U');
CMDOPT_REGFN(const char* ,  Str, 'S');
CMDOPT_REGFN(        bool, Bool, 'B');

// The command-line option list is in reverse order, this prints it out in
// "normal" order through simple / direct recursion.
//
CMD_OPT_STGCLS
void print_usage_reversed(cmdOptDesc* desc, FILE* dfp)
#if CMD_OPT_DEFS
{
  if (desc->codNextOpt)
    print_usage_reversed(desc->codNextOpt, dfp);

  fprintf(dfp, "  " CMDSWT_STR "%c --%-16s", desc->codLetter, desc->codLongNm);
  if (desc->codValArg)
    fprintf(dfp, "=%-12s", desc->codValArg);
  else
    fprintf(dfp, "             ");

  fprintf(dfp, "%c ", (desc->codType == 'B' && desc->codDefVal.codDefValBool) ? '*' : ' ');

  const char* typestr = "????";
  switch(desc->codType) {
  case 'I': typestr =  "int"; break;
  case 'U': typestr = "uint"; break;
  case 'S': typestr =  "str"; break;
  case 'B': typestr = "bool"; break;
  }
  fprintf(dfp, " (%4s) ... %s", typestr, desc->codInfoStr);

  switch(desc->codType) {
  case 'I':
    if (desc->codDefVal.codDefValInt != 0)
      fprintf(dfp, " (default=%d)" , desc->codDefVal.codDefValInt);
    break;
  case 'U':
    if (desc->codDefVal.codDefValUint != 0)
      fprintf(dfp, " (default=%u)" , desc->codDefVal.codDefValUint);
    break;
  case 'S':
    if (desc->codDefVal.codDefValStr != nullptr && *desc->codDefVal.codDefValStr != 0)
      fprintf(dfp, " (default='%s')", desc->codDefVal.codDefValStr);
    break;
  case 'B':
    break;
  }
  fprintf(dfp, "\n");
}
#endif // CMD_OPT_DEFS
;

CMD_OPT_STGCLS
void CMDOPT_PRINT_USAGE(FILE* dfp)
#if CMD_OPT_DEFS
{
  print_usage_reversed(cmdOptDescList, dfp);

  fprintf(stderr, "\n");
  fprintf(stderr, "  " CMDSWT_STR  "?   show this info\n");
  fprintf(stderr, "  " CMDSWT_STR  "h   show this info\n");
}
#endif // CMD_OPT_DEFS
;

// REVIEW peterk: how *should* we report errors to the client? this isn't very clean.
extern void usage();

//////////////////////////////////////////////////////////////////////////////////
//
// Collects an identifier from the given string; a heap buffer with the identifier
// is returned in '*nameOut' if successful.
//
CMD_OPT_STGCLS
bool slurpIdentifier(const char*  str4err,
                     const char* *strAddr,
                     unsigned     delimCh, const char* *nameOut)
#if CMD_OPT_DEFS
{
  /* Slurp characters until we hit the end / delimiter / non-ident */
  const char*   str = *strAddr;
  const char*   beg = str;

  for (;;) {
    /* Pull the next character and see what we have */
    unsigned      nch = *str;
    if (nch == 0) {
      if (delimCh != 0) {
        // REVIEW peterk: should this be an error?
        printf("WARNING: while slurping ident from '%s' using a delimiter of '%c', we hit the end of the string\n", *strAddr, delimCh);
      }

      /* Let's assume that this is OK */
      break;
    }

    if (nch == delimCh) {
      /* Nice and sweet */
      break;
    }

    /* This better be part of the identifier */
    if (nch != '_' && !isalpha(nch)) {
      /* Digits are OK, but not as the first char */
      if (str == beg || !isdigit(nch)) {
        /* This is OK if there is no single delimiter */
        if (delimCh != 0) {
          fprintf(stderr, "ERROR: expected identifier character at position %u in '%s'\n", (unsigned)(str - str4err), str4err);
          return false;
        }
        break;
      }
    }

    str++;
  }

  /* Save a copy of the identifier in a permanent location */
  unsigned      len = str - beg;
  char*         ids = (char *) malloc(sizeof(char)*len + 1);  // REVIEW peterk: memory leak (?)

  memcpy(ids, beg, len); ids[len] = 0;

  /* The source pointer should be on the delimiter or 0 */
  assert(*str == 0 || (unsigned)*str == delimCh || delimCh == 0);

  /* Let the caller figure out if the delimiter is present or not */
  *strAddr = str;
  *nameOut = ids;

  return true;
}
#endif // CMD_OPT_DEFS
;

//////////////////////////////////////////////////////////////////////////////////
//
// Copies a bunch of characters/bytes to the heap.
//
CMD_OPT_STGCLS
char* saveStringAsIs(const char* str, unsigned len)
#if CMD_OPT_DEFS
{
  #ifdef __cplusplus
  //char*     save = new char[len + 1];
  #else
  char* save = malloc(sizeof(char)*len +1);
  #endif
  memcpy(save, str, len); save[len] = 0;
  return save;
}
#endif // CMD_OPT_DEFS
;

// Copies a bunch of characters/bytes to the heap, optionally strips quotes.
CMD_OPT_STGCLS
char* saveString(const char* str, unsigned len, bool stripQuotes)
#if CMD_OPT_DEFS
{
  if (stripQuotes && len >= 2) {
    char        ch1 = *str;
    /* If the value starts and ends with the same quote ... */
    if ((ch1 == '\'' || ch1 == '\"') && str[len-1] == ch1) {
      /* ... get rid of the leading and ending quote */
      str += 1;
      len -= 2;
    }
  }

  return saveStringAsIs(str, len);
}
#endif // CMD_OPT_DEFS
;

////////////////////////////////////////////////////////////////////////////////////////
//
// Process a string-valued command-line option
//
CMD_OPT_STGCLS
bool processOneCmdOptStr(const char*  fullStr,       // "full" original option string
                         const char*  optStr,        // just past the option name
                         bool         valRequired,   // use for numeric options only
                         bool         stripQuotes,   // remove '' or "" if present
                         const char* *optionValAddr) // address of option value
#if CMD_OPT_DEFS
{
  /* Check to see if there is a value */
  if (*optStr == 0) {
    /* There is no value - is that acceptable? */
    if (valRequired) {
      fprintf(stderr, "ERROR: The '%s' command-line option requires a value\n\n", fullStr);
      return false;
    }

    if (optionValAddr != nullptr) *optionValAddr = "";
    return true;
  }

  /* Skip '=' if present; it's unlikely to be part of the actual value */
  if (*optStr == '=')
    optStr++;

  /* Check for quotes we may need to strip */
  unsigned    len = strlen(optStr);
  if (stripQuotes && len >= 2) {
    char        ch1 = *optStr;
    /* If the value starts and ends with the same quote */
    if ((ch1 == '\'' || ch1 == '\"') && optStr[len-1] == ch1) {
      /* Get rid of the leading quote and its copy at the end */
      optStr += 1;
      len    -= 2;
    }
  }

  /* Hopefully the caller wants to see the value */
  if (optionValAddr != nullptr)
    *optionValAddr = saveString(optStr, len, stripQuotes); // REVIEW: memory leak

  return true;
}
#endif // CMD_OPT_DEFS
;

////////////////////////////////////////////////////////////////////////////////////////
//
// Process a boolean/flag or numeric (aka 'scalar') command-line option
//
CMD_OPT_STGCLS
bool processOneCmdOptScl(const char*  fullStr,       // "full" original option string
                         const char*  optStr,        // just past the option character
                         bool         valRequired,   // use for numeric options only
                         unsigned    *toggleUnsAddr, // address of value if numeric
                         bool        *toggleFlgAddr) // address of value if boolean
#if CMD_OPT_DEFS
{
  /* Grab the next character and see what we have */
  unsigned      nxtCh = *optStr++;

  /* Is this all there is? */
  if (nxtCh == 0) {
    /* Saying "-x" means enable/increment the flag */
    if (!valRequired) {
      if (toggleUnsAddr != nullptr) *toggleUnsAddr += 1;
      if (toggleFlgAddr != nullptr) *toggleFlgAddr  = true;
      return true;
    } else {
      fprintf(stderr, "ERROR: The '%s' command-line option requires a value\n\n", fullStr);
      return false;
    }
  }

  /* Are we disabling the option? */
  if (nxtCh == '-') {
    /* Saying "-x-" means disable/reset the flag */
    if (*optStr == 0) {
      if (toggleUnsAddr != nullptr) *toggleUnsAddr = 0;
      if (toggleFlgAddr != nullptr) *toggleFlgAddr = false;
      return true;
    }

    /* We have "-x-blah" which is not currently meaningful */
    fprintf(stderr, "ERROR: Invalid command-line option '%s'\n\n", fullStr);
    return false;
  }

  /* Skip '=' if present; we assume it's not part of the actual value */
  if (nxtCh == '=')
    nxtCh = *optStr++;

  /* If we allow numbers, check for a leading digit */
  if (toggleUnsAddr != nullptr && isdigit(nxtCh)) {
    /* Back up to get back to the first digit of the value */
    optStr--;

    /* We have 'optStr' pointing at the value; convert it to a binary int */
    const char* endptr;
    long        lval = strtol(optStr, (char**)&endptr, 10);

    if (lval < 0) {
      fprintf(stderr, "ERROR: Expected positive numeric value in command-line option '%s'\n\n", fullStr);
      return false;
    }

    *toggleUnsAddr = (unsigned)lval;
    return true;
  }

  /* Nothing else makes sense, right? */
  fprintf(stderr, "ERROR: Invalid command-line option '%s'\n\n", fullStr);
  return false;
}
#endif // CMD_OPT_DEFS
;

/* Process all options on the command line; updates argc/argv */
CMD_OPT_STGCLS
bool processCmdOpts(int *argcPtr, const char ***argvPtr)
#if CMD_OPT_DEFS
{
  int           argc = *argcPtr;
  const char * *argv = *argvPtr;

  FILE*         resp = nullptr;  // non-NULL when reading options from @response_file
  char          rtmp[256];       // the line buffer size is basically arbitrary

  const char*   rfnm = nullptr;  // response file path (if any)
  unsigned      line = 0;        // next line # for response file

  while (argc > 0 || resp != nullptr)
  {
    const char*   optStr = nullptr;

    /* Are we slurping options from a response file? */
    if (resp != nullptr) {
      /* Get the next line from the response file */
      ++line;

      if (fgets(rtmp, sizeof(rtmp), resp) == nullptr) {
        /* This is either a read error or the end of the file */
        if (ferror(resp)) {
          fprintf(stderr, "ERROR: could not read from response file '%s'\n", rfnm);
          exit(1);
        }

        /* We've reached the end of the response file */
        fclose(resp); resp = nullptr; rfnm = nullptr;
        continue;
      }

      /* Remove the terminator char(s) if present */
      unsigned    llen = strlen(rtmp);
      if (llen > 0 && rtmp[llen - 1] == '\n')
        rtmp[--llen] = 0;
      if (llen > 0 && rtmp[llen - 1] == '\r')
        rtmp[--llen] = 0;

      /* Point at the line and skip any whitespace */
      optStr = rtmp;
      while (isspace(*optStr))
        optStr++;

      /* Ignore blank lines */
      if (*optStr == 0)
        continue;
    } else {
      /* Grab the next option from the command line */
      optStr = *argv;

      /* Do we have a response file slurp? */
      if (*optStr == '@') {
        /* Save the file path in case we get an error later */
        rfnm = ++optStr;

#ifdef _WIN32
        /* On Windows we explicitly ask for "text" mode */
        errno_t     err = fopen_s(&resp, rfnm, "rt");
        if (err != 0) {
          fprintf(stderr, "ERROR: could not open response file '%s' (error %d)\n", rfnm, err);
          exit(1);
        }
#else
        resp = fopen(rfnm, "r");
        if (resp == nullptr) {
          fprintf(stderr, "ERROR: could not open response file '%s')\n", rfnm);
          exit(1);
        }
#endif

        /* Next time around the loop we'll start reading this response file */
        line = 0;

        argc--;
        argv++;
        continue;
      }
    }

    /* Is this an option? */
    if (*optStr != CMDSWT_CH
#ifdef _WIN32
                             && *optStr != '-'     /* .. we allow both '/' and '-' on Windows */
#endif
                                              ) {
      /* Not an option but it could be a response file comment */
      if (resp != nullptr ) {
        if (*optStr != '#') {
          /* This is not allowed inside a response file */
          fprintf(stderr, "ERROR: line %u in response file '%s' not recognized - '%s'\n", line, rfnm, rtmp);

          fclose(resp);
          exit(1);
        }

        /* Ignore the response file comment line and keep going */
        continue;
      }

      /* Stop when we reach the first command line non-option argument */
      assert(resp == nullptr);
      break;
    }

    /* Save a ref to the first character of the option (for error messages) */
    const char*   orgStr = optStr;

    /* Check for a '--longname' style option (yes, it's '--' even on Windows) */
    int           let_opt = 0;
    const char*   str_opt = nullptr;

    if (optStr[0] == '-' && optStr[1] == '-') {
      /* Skip over the '--' and grab the 'long' option name */
      optStr += 2;
      if (!slurpIdentifier(orgStr, &optStr, 0, &str_opt))
        return false;
    } else {
      /* Skip the '-' or '/' and get the option letter */
      ++optStr;
      let_opt = *optStr;
      ++optStr;
    }

    /* Skip over the current argument in argv[] */
    if (!resp) {
      argc--;
      argv++;
    }

    /* Look for a match in the option table */
    cmdOptDesc*   desc;
    for (desc = cmdOptDescList; desc; desc = desc->codNextOpt) {
      /* Are we matching by single letter or full string name? */
      if (let_opt != 0) {
        if (desc->codLetter == let_opt)
          break;
      } else {
        if (strcmp(desc->codLongNm, str_opt) == 0)
          break;
      }
    }

    if (desc == nullptr) {
      /* This means we didn't find a match in the option table */
      fprintf(stderr, "ERROR: Unrecognized command-line option '%s'\n\n", orgStr);
      usage();

      return false;
    }

    /* We have a match; process the option and keep the value for validation */
  //int           opt_val_I = 0;       // REVIEW: add validation for int  values
  //bool          opt_val_B = false;   // REVIEW: add validation for bool values
    const char*   opt_val_S = nullptr;

    /* Do we have a string or scalar-valued option? */
    if (desc->codType == 'S') {
      if (!processOneCmdOptStr(orgStr, optStr, true, true, desc->codVarPtr.codVarPtrStr)) {
        usage();
        return false;
      }
      opt_val_S = *desc->codVarPtr.codVarPtrStr;
    } else {
      /* Booleans vs. numerical values are the two main categories */
      if (desc->codType == 'B') {
        if (!processOneCmdOptScl(orgStr, optStr, false, nullptr, desc->codVarPtr.codVarPtrBool)) {
          usage();
          return false;
        }
    //--opt_val_B = *desc->codVarPtr.codVarPtrBool;
      } else {
        if (!processOneCmdOptScl(orgStr, optStr,  true, desc->codVarPtr.codVarPtrUint, nullptr)) {
          usage();
          return false;
        }
    //--opt_val_I = *desc->codVarPtr.codVarPtrUint;
      }
    }

    /* If there is an option value validator callback, invoke it */
    if (desc->codValidate != nullptr) {
      assert(desc->codType == 'S'); // REVIEW peterk: will need to handle non-string values someday!

      /* Call the client's validation function pointer */
      if (!(*desc->codValidate)(orgStr, opt_val_S))
        return false;
    }
  }

  *argcPtr = argc;
  *argvPtr = argv;

  return true;
}
#endif // CMD_OPT_DEFS
;

//////////////////////////////////////////////////////////////////////////////////
//
// The following helpers collect numbers from given input strings.
//
CMD_OPT_STGCLS
bool slurpUnsigned(const char* str4err, const char* *strAddr, unsigned *valOut)
#if CMD_OPT_DEFS
{
  const char*   str = *strAddr;
  unsigned      val = 0;

  if (!isdigit(*str)) {
    fprintf(stderr, "ERROR: expected numeric value at position %u in '%s'\n", (unsigned)(str - str4err), str4err);
    return false;
  }

  /* We don't check for overflow, BTW - left as an exercise to the user */
  do {
    assert(isdigit(*str));
    val = val * 10 + (*str++ - '0');
  }
  while (isdigit(*str));

  /* Things went well, return the value (and the update position) */
  *valOut  = val;
  *strAddr = str;
  return true;
}
#endif // CMD_OPT_DEFS
;

CMD_OPT_STGCLS
bool slurpSigned(const char* str4err, const char* *strAddr, int *valOut)
#if CMD_OPT_DEFS
{
  const char*   str = *strAddr;
  bool          neg = false;
  unsigned      val;

  if (*str == '-') {
    neg = true;
    str++;
  }

  if (!slurpUnsigned(str4err, &str, &val))
    return false;

  *strAddr = str;
  *valOut  = neg ? -val : +val;
  return true;
}
#endif // CMD_OPT_DEFS
;

//////////////////////////////////////////////////////////////////////////////////
//
// Ingests either a simple number (interpreted as a total size in bytes), or
// an optionally parenthesized list of dimensions. When 'sizeOut' is non-NULL
// and a single number is found, it's returned in *sizeOut. Otherwise, a list
// of dimensions must be present; these are stored at dimsOut[0], dimsOut[1],
// etc (and the max number of dimensions is hard-wired to be 3).
//
CMD_OPT_STGCLS
bool slurpSizeOrDims(const char*  str4err,
                     const char* *strAddr,
                     unsigned     sizePer,
                     int          endgCh, size_t *sizeOut,
                                          size_t *dimsOut)
#if CMD_OPT_DEFS
{
  const char*   str = *strAddr;

  unsigned      dim;
  if (!slurpUnsigned(str4err, &str, &dim))
    return false;

  /* The following is probably redundant, but let's be safe */
  dimsOut[0] =
  dimsOut[1] =
  dimsOut[2] = 0;

  /* If there is only one number, it means 'total size in bytes' */
  if (*str == endgCh && endgCh != 0 && sizeOut != nullptr) {
    *strAddr = str + 1;
    *sizeOut = dim;
    return true;
  }

  /* Are we allowed to have more than one number? */
  if (endgCh == 0 && sizeOut != nullptr) {
    /* Let the caller sort out what follows the number */
    *strAddr = str;
    *sizeOut = dim;
    return true;
  }

  /* Grab one or two additional dimensions */
  unsigned      cnt = 1;

  dimsOut[0] = dim;

  for(;;) {
    if (*str != ',') {
      fprintf(stderr, "ERROR: expected ',' or '%c' at position %u in argument '%s'\n", endgCh, (unsigned)(str - *strAddr), *strAddr);
      return false;
    }
    str++;

    if (cnt >= 3) {
      fprintf(stderr, "ERROR: too many dimensions at position %u in argument '%s'\n", (unsigned)(str - *strAddr), *strAddr);
      return false;
    }

    if (!slurpUnsigned(str4err, &str, &dim))
      return false;

    dimsOut[cnt++] = dim;

    if (*str == endgCh || *str == 0)
      break;
  }

  /* Compute the total size */
  dim = dimsOut[0] * dimsOut[1];
  if (dimsOut[2] != 0)
    dim *= dimsOut[2];

  if (sizePer > 1)
    dim *= sizePer;

  if (sizeOut != nullptr) {
    *sizeOut = dim;
  }

  *strAddr = str + 1;
  return true;
}
#endif // CMD_OPT_DEFS
;

#undef CMD_OPT_ONE_LOCAL_COPY
#undef CMD_OPT_ONE_SHARED_COPY
#undef CMD_OPT_DEFS
//#undef CMD_OPT_STGCLS

//#endif // __RUNTIME_CMDOPTPARSER_H__

