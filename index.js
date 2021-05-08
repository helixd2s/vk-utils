const parseXml = require('@rgrove/parse-xml');
const fs = require('fs');

let doc = parseXml(fs.readFileSync("../Vulkan-Docs/xml/vk.xml").toString("utf-8"));

let registry = doc.children.find((e,i)=>{ return e.type == "element" && e.name == "registry"; });

let types = registry.children.find((e,i)=>{ return e.type == "element" && e.name == "types"; });

let structs = types.children.filter((e,i)=>{ return e.type == "element" && e.name == "type" && e.attributes["category"] == "struct"; });

let extract = (ec)=>{
    return ec.children.find((en,ind)=>{ return en.type=="text"; }).text;
};

let memberByName = (e,name)=>{
    return e.children.find((em,im)=>{
        if (em.type == "element" && em.name == "member") {

            return !!em.children.find((ec,ic)=>{ 
                return ec.type=="element" && ec.name=="name" && extract(ec).includes(name);
            });

        };
        return false;
    });
};

let hasSType = structs.filter((e,i)=>{ 
    return !!memberByName(e,"sType");
});

let sTypeMap = {};
hasSType.map((e,i)=>{
    let sType = memberByName(e,"sType");
    let defVal = sType.attributes["values"];
    if (defVal) {
        sTypeMap[defVal] = e.attributes["name"];
    };
});


let usedBy = {};

let extensions = registry.children.find((e,i)=>{ return e.type == "element" && e.name == "extensions"; });

extensions.children.filter((e,i)=>{
    return e.type == "element" && e.name == "extension";
}).map((e,i)=>{
    let extName = e.attributes["name"];
    
    e.children.filter((ec,ic)=>{  return ec.type == "element" && ec.name == "require"; }).map((ec,ic)=>{
        ec.children.filter((em,im)=>{ return em.type == "element" && em.name == "type"; }).map((em,im)=>{
            usedBy[em.attributes["name"]] = extName;
        });
    });
});

console.log(usedBy);


let cases = ()=>{
    let cases = [];
    for (let k in sTypeMap) {
        let s = sTypeMap[k];
        let by = usedBy[s];
        if (by) { cases.push(`#ifdef ${by}`); };
        cases.push(`        case ${k}: return sizeof(${s}); break;`);
        if (by) { cases.push(`#endif`); }; 
    }
    return cases.join("\n");
};

let sizer = `
#include <vulkan/vulkan.h>

size_t vkGetStructureSizeBySType(VkStructureType sType) {
    switch(sType) {
${cases()}
        default: 
    };
    return 0ull;
};

`;


fs.writeFileSync("./sizes.h", /*JSON.stringify(sTypeMap, null, '\t')*/ sizer);
