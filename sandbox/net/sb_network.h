//
//  sb_network.h
//  Battleships
//
//  Created by Andrey Kunitsyn on 01/09/14.
//
//

#ifndef __Battleships__sb_network__
#define __Battleships__sb_network__

#include <ghl_net.h>
#include <sbstd/sb_string.h>
#include <sbstd/sb_vector.h>
#include <sbstd/sb_pointer.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_intrusive_ptr.h>
#include "meta/sb_meta.h"
#include "sb_data.h"
#include "sb_image.h"
#include "sb_multi_stream.h"

namespace GHL {
    struct WriteStream;
}

namespace Sandbox {
    
    class Resources;
    
    class NetworkRequestBase : public GHL::NetworkRequest, public meta::object {
        SB_META_OBJECT
    public:
        explicit NetworkRequestBase(const sb::string& url);
        ~NetworkRequestBase();
        /// ghl counter
        /// add reference
        virtual void GHL_CALL AddRef() const SB_OVERRIDE;
        /// release reference
        virtual void GHL_CALL Release() const SB_OVERRIDE;
        /// ghl interface
        /// url
        virtual const char* GHL_CALL GetURL() const SB_OVERRIDE;
        /// headers
        virtual GHL::UInt32 GHL_CALL GetHeadersCount() const SB_OVERRIDE;
        virtual const char* GHL_CALL GetHeaderName(GHL::UInt32 idx) const SB_OVERRIDE;
        virtual const char* GHL_CALL GetHeaderValue(GHL::UInt32 idx) const SB_OVERRIDE;
        
        /// received response
        virtual void GHL_CALL OnResponse(GHL::UInt32 status) SB_OVERRIDE;
        /// received header
        virtual void GHL_CALL OnHeader(const char* name,const char* value) SB_OVERRIDE;
        /// received data
        virtual void GHL_CALL OnData(const GHL::Byte* data,GHL::UInt32 size) SB_OVERRIDE;
        /// received complete
        virtual void GHL_CALL OnComplete() SB_OVERRIDE;
        /// received error
        virtual void GHL_CALL OnError(const char* error) SB_OVERRIDE;
        
        bool GetError() const { return m_error; }
        bool GetCompleted() const { return m_completed; }
        GHL::UInt32 GetStatusCode() const { return m_status_code; }
        size_t GetReceivedSize() const { return m_received_size; }
        typedef sb::map<sb::string, sb::string> headers_map_t;
        
        void SetHeader(const sb::string& name, const sb::string& value) { m_headers[name]=value; }
        const headers_map_t& GetReceivedHeaders() const { return m_received_headers; }
        
        const sb::string& GetErrorText() const { return m_error_text; }
    private:
        sb::string  m_url;
        headers_map_t   m_headers;
        bool    m_completed;
        bool    m_error;
        GHL::UInt32 m_status_code;
        size_t  m_received_size;
        headers_map_t   m_received_headers;
        sb::string  m_error_text;
    };
    typedef sb::intrusive_ptr<NetworkRequestBase> NetworkRequestBaseBtr;
    
    class NetworkRequest : public NetworkRequestBase {
        SB_META_OBJECT
    public:
        explicit NetworkRequest(const sb::string& url) : NetworkRequestBase(url) {}
        const sb::string& GetData() const { return m_data; }
        /// received data
        virtual void GHL_CALL OnData(const GHL::Byte* data,GHL::UInt32 size) SB_OVERRIDE;
    protected:
        void SetData(const sb::string& d) { m_data = d; }
        void AppendData(const char* data,size_t size) { m_data.append(data,size); }
    private:
        sb::string  m_data;
    };
    
    typedef sb::intrusive_ptr<NetworkRequest> NetworkRequestPtr;
    
    class NetworkDataRequest : public NetworkRequestBase {
        SB_META_OBJECT
    public:
        explicit NetworkDataRequest(const sb::string& url) :  NetworkRequestBase(url), m_data(0) {}
        ~NetworkDataRequest();
        const GHL::Data*    GetData() const { return m_data; }
        /// received data
        virtual void GHL_CALL OnData(const GHL::Byte* data,GHL::UInt32 size) SB_OVERRIDE;
    protected:
        void ReleaseData();
    private:
        VectorData<GHL::Byte>*   m_data;
    };
    
    class NetworkFileRequest : public NetworkRequestBase {
        SB_META_OBJECT
    public:
        explicit NetworkFileRequest(const sb::string& url, GHL::WriteStream* wd);
        ~NetworkFileRequest();
        virtual void GHL_CALL OnData(const GHL::Byte* data,GHL::UInt32 size) SB_OVERRIDE;
        virtual void GHL_CALL OnComplete() SB_OVERRIDE;
    protected:
        void ReleaseData();
        GHL::WriteStream*   m_ds;
    };
    typedef sb::intrusive_ptr<NetworkFileRequest> NetworkFileRequestPtr;
    
#ifndef SB_NO_RESOURCES
    class ImageRequest : public NetworkDataRequest {
        SB_META_OBJECT
    public:
        explicit ImageRequest(const sb::string& url,Resources* res) :  NetworkDataRequest(url),m_resources(res) {}
        const ImagePtr& GetImage() const { return m_img; }
        virtual void GHL_CALL OnComplete() SB_OVERRIDE;
    private:
        ImagePtr    m_img;
        Resources*  m_resources;
    };
    
    typedef sb::intrusive_ptr<ImageRequest> ImageRequestPtr;
#endif
    
    class NetworkPostData : public meta::object {
        SB_META_OBJECT
    public:
        NetworkPostData();
        virtual ~NetworkPostData();
        GHL::Data* GetData() { return m_data; }
        virtual void Setup(NetworkRequestBase* request) = 0;
    protected:
        VectorData<GHL::Byte>* m_data;
    };
    typedef sb::intrusive_ptr<NetworkPostData> NetworkPostDataPtr;
    
    class NetworkMultipartFormData : public NetworkPostData {
        SB_META_OBJECT
    public:
        NetworkMultipartFormData();
        ~NetworkMultipartFormData();
        virtual void Setup(NetworkRequestBase* request) SB_OVERRIDE;
        void AddFile(const sb::string& name,
                     const sb::string& filename,
                     const sb::string& content_type,
                     const BinaryDataPtr& data);
        void AddFormField(const sb::string& field, const sb::string& data);
        void Close();
    private:
        sb::string  m_boundary;
        void write_boundary();
        void write_string(const char* str);
    };
    
    typedef sb::intrusive_ptr<NetworkMultipartFormData> NetworkMultipartFormDataPtr;
    
    
    class NetworkMultipartFormStream : public MultiStream {
        SB_META_OBJECT
    private:
        sb::string m_boundary;
        
        
        void write_boundary();
        
    public:
        NetworkMultipartFormStream();
        
        
        void Setup(NetworkRequestBase* request);
        void AddStream(const sb::string& name,
                     const sb::string& filename,
                     const sb::string& content_type,
                     GHL::DataStream* data);
        void AddFormField(const sb::string& field, const sb::string& data);
        void AddFormField(const sb::string& name,
                          const sb::string& filename,
                          const sb::string& content_type,
                          const sb::string& data);
        void Close();
    };
    typedef sb::intrusive_ptr<NetworkMultipartFormStream> NetworkMultipartFormStreamPtr;
    
    class Network : public meta::object {
        SB_META_OBJECT
    public:
        explicit Network(Resources* res);
        ~Network();
#ifndef SB_NO_RESOURCES
        ImageRequestPtr GETImage(const sb::string& url);
#endif
        NetworkRequestPtr SimpleGET(const sb::string& url);
        NetworkRequestPtr SimplePOST(const sb::string& url, const sb::string& data);
        NetworkRequestPtr POST(const sb::string& url,
                               const NetworkPostDataPtr& data);
        NetworkRequestPtr POSTFormStream(const sb::string& url,
                              const NetworkMultipartFormStreamPtr& data);
        NetworkFileRequestPtr GETFile(const sb::string& url,const sb::string& filename);
        void SetCookie(const sb::string& host,
                       const sb::string& cookie) {
            m_cookie[host]=cookie;
        }
        void Process();
    private:
        GHL::Network* m_net;
        Resources*  m_resources;
        sb::map<sb::string,sb::string> m_cookie;
    protected:
        GHL::Network* GetNet() { return m_net; }
        void ApplyCookie(const NetworkRequestBaseBtr& req);
    };
    
}

#endif /* defined(__Battleships__sb_network__) */
