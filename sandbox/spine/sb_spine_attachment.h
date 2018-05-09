#ifndef SB_SPINE_ATTACHMENT_H_INCLUDED
#define SB_SPINE_ATTACHMENT_H_INCLUDED


#include <spine/AttachmentLoader.h>
#include <spine/RegionAttachment.h>
#include <spine/MeshAttachment.h>
#include <spine/WeightedMeshAttachment.h>

#include "sb_transform2d.h"
#include "sb_image.h"

namespace Sandbox {
    
    struct SpineImageAttachment : public spRegionAttachment {
        explicit SpineImageAttachment(const char* name);
        ImagePtr image;
        Transform2d tr;
    };

    struct SpineWeightedMeshAttachment : public spWeightedMeshAttachment {
        explicit SpineWeightedMeshAttachment(const char* name);
        ImagePtr image;
        int vertices_index;
    };

    struct SpineAttachmentLoader : spAttachmentLoader {
        spAtlas* atlas;
        explicit SpineAttachmentLoader(spAtlas* atlas);
        ~SpineAttachmentLoader();
    };

}

#endif /*SB_SPINE_ATTACHMENT_H_INCLUDED*/
