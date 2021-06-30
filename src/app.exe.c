#include <windows.h>
#include <stdio.h>

typedef int Z;
#define null NULL

const char *help = ("put-resource exeName --id number --type number --value fileName\n"
  "  put resource w/ specific { id } and { type } and value from { fileName } to { exeName } file"
);

char *getFileData(char *fileName, Z *optSize) {
  FILE *f = fopen(fileName, "rb");
  if(f == null) {
    return null;
  }
  else {
    fseek(f, 0, SEEK_END);
    Z fileSize = ftell(f);
    if(optSize != null) {
      *optSize = fileSize;
    }
    char *out = malloc(fileSize);
    fseek(f, 0, SEEK_SET);
    fread(out, fileSize, 1, f);
    fclose(f);
    return out;
  }
}

Z main(Z argc, char *argv[]) {
  if(argc > 2) {
    char *exeName = argv[1];
    Z id = -1;
    Z type = -1;
    char *v = null;
    Z vSize = -1;
    Z i = 1; while(++i < argc) {
      #define matchOption(v) do { const char *option = v; if(0) {  } 
      #define option(name) else if(strcmp(argv[i], name) == 0)
      #define otherOption else
      #define endMatchOption } while(0);
      matchOption(argv[i])
        option("--id") {
          id = atoi(argv[++i]);
        }
        option("--type") {
          type = atoi(argv[++i]);
        }
        option("--value") {
          v = getFileData(argv[++i], &vSize);
        }
        otherOption {
          printf("Unknown option %s\n", argv[i]);
          goto helpLabel;
        }
      endMatchOption
    }
    
    if(id == -1) {
      printf("Missed id");
      goto helpLabel;
    }
    else if(type == -1) {
      printf("Missed type");
      goto helpLabel;
    }
    else if(v == null) {
      printf("Missed value");
      goto helpLabel;
    }
    else {
      // printf("type = %i, id = %i, v = %s, vSize = %i", type, id, v, vSize);
      // [http://msdn.microsoft.com/en-us/library/windows/desktop/ms648008(v=vs.85).aspx#_win32_Updating_Resources]
      HANDLE hUpdateRes = BeginUpdateResource(exeName, FALSE);
      
      // Add the dialog box resource to the update list.
      BOOL result = UpdateResource(hUpdateRes,    // update resource handle
          MAKEINTRESOURCE(type),                         // change dialog box resource
          MAKEINTRESOURCE(id),         // dialog box id
          MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  // neutral language
          v,                         // ptr to resource info
          vSize // size of resource info
      );       

      free(v);
      
      if(result == FALSE) {
        printf("Could not add resource.");
        return 1;
      }

      // Write changes to FOOT.EXE and then close it.
      if (!EndUpdateResource(hUpdateRes, FALSE)) {
        printf("Could not write changes to file.");
        return 1;
      }

      return 0;
    }
  }
  else {
    goto helpLabel;
  }
  
  helpLabel: {
    printf(help);
    return 1;
  }
}