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
#include "sb_data.h"
#include "sb_image.h"

namespace Sandbox {
    
    class Resources;
    
    class NetworkRequestBase : public sb::ref_counter_not_copyable<GHL::NetworkRequest> {
    public:
        explicit NetworkRequestBase(const sb::string& url);
        ~NetworkRequestBase();
        /// ghl counter
        /// add reference
        virtual void GHL_CALL AddRef() const;
        /// release reference
        virtual void GHL_CALL Release() const;
        /// ghl interface
        /// url
        virtual const char* GHL_CALL GetURL() const;
        /// headers
        virtual GHL::UInt32 GHL_CALL GetHeadersCount() const;
        virtual const char* GHL_CALL GetHeaderName(GHL::UInt32 idx) const;
        virtual const char* GHL_CALL GetHeaderValue(GHL::UInt32 idx) const;
        
        /// received response
        virtual void GHL_CALL OnResponse(GHL::UInt32 status);
        /// received header
        virtual void GHL_CALL OnHeader(const char* name,const char* value);
        /// received data
        virtual void GHL_CALL OnData(const GHL::Byte* data,GHL::UInt32 size);
        /// received complete
        virtual void GHL_CALL OnComplete();
        /// received error
        virtual void GHL_CALL OnError(const char* error);
        
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
    
    class NetworkRequest : public NetworkRequestBase {
    public:
        explicit NetworkRequest(const sb::string& url) : NetworkRequestBase(url) {}
        const sb::string& GetData() const { return m_data; }
        /// received data
        virtual void GHL_CALL OnData(const GHL::Byte* data,GHL::UInt32 size);
    private:
        sb::string  m_data;
    };
    
    typedef sb::intrusive_ptr<NetworkRequest> NetworkRequestPtr;
    
    class NetworkDataRequest : public NetworkRequestBase {
    public:
        explicit NetworkDataRequest(const sb::string& url) :  NetworkRequestBase(url), m_data(0) {}
        ~NetworkDataRequest();
        const GHL::Data*    GetData() const { return m_data; }
        /// received data
        virtual void GHL_CALL OnData(const GHL::Byte* data,GHL::UInt32 size);
    protected:
        void ReleaseData();
    private:
        VectorData<GHL::Byte>*   m_data;
    };
    
#ifndef SB_NO_RESOURCES
    class ImageRequest : public NetworkDataRequest {
    public:
        explicit ImageRequest(const sb::string& url,Resources* res) :  NetworkDataRequest(url),m_resources(res) {}
        const ImagePtr& GetImage() const { return m_img; }
        virtual void GHL_CALL OnComplete();
    private:
        ImagePtr    m_img;
        Resources*  m_resources;
    };
    
    typedef sb::intrusive_ptr<ImageRequest> ImageRequestPtr;
#endif
    
    class NetworkPostData : public sb::ref_countered_base_not_copyable {
    public:
        NetworkPostData();
        virtual ~NetworkPostData();
        GHL::Data* GetData() { return m_data; }
        virtual void Setup(NetworkRequestBase* request) = 0;
    protected:
        VectorData<GHL::Byte>* m_data;
    };
    
    class NetworkMultipartFormData : public NetworkPostData {
    public:
        NetworkMultipartFormData();
        ~NetworkMultipartFormData();
        virtual void Setup(NetworkRequestBase* request);
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
    
    typedef sb::intrusive_ptr<NetworkPostData> NetworkPostDataPtr;
    
    class Network {
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
        void SetCookie(const sb::string& host,
                       const sb::string& cookie) {
            m_cookie[host]=cookie;
        }
        void Process();
    private:
        GHL::Network* m_net;
        Resources*  m_resources;
        sb::map<sb::string,sb::string> m_cookie;
        void ApplyCookie(const NetworkRequestPtr& req);
    };
    
}

#endif /* defined(__Battleships__sb_network__) */
