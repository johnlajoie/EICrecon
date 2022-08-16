// Copyright 2022, David Lawrence
// Subject to the terms in the LICENSE file found in the top-level directory.
//
//

#include "JEventSourcePODIO.h"
#include "JEventSourcePODIOsimple.h"
//#include "EICRootWriter.h"
#include "EICRootWriterSimple.h"


// Make this a JANA plugin
extern "C" {
void InitPlugin(JApplication *app) {
    InitJANAPlugin(app);
    app->Add(new JEventSourceGeneratorT<JEventSourcePODIO>());
    app->Add(new JEventSourceGeneratorT<JEventSourcePODIOsimple>());

    // Only add a EICRootWriter if the user has specified a configuration parameter relevant to writing
    if( app->GetJParameterManager()->Exists("PODIO:OUTPUT_FILE")
        ||  app->GetJParameterManager()->Exists("PODIO:OUTPUT_FILE_COPY_DIR")
        ||  app->GetJParameterManager()->Exists("PODIO:OUTPUT_INCLUDE_COLLECTIONS")
        ||  app->GetJParameterManager()->Exists("PODIO:OUTPUT_EXCLUDE_COLLECTIONS")        ){
        app->Add(new EICRootWriterSimple());
    }
}
}

