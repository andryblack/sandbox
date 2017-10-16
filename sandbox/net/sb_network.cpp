//
//  sb_network.cpp
//  Battleships
//
//  Created by Andrey Kunitsyn on 01/09/14.
//
//

#include "sb_network.h"
#include "sb_data.h"
#include <ghl_data.h>
#include <ghl_data_stream.h>
#include "sb_log.h"
#include "sb_resources.h"

SB_META_DECLARE_OBJECT(Sandbox::Network, Sandbox::meta::object)
SB_META_DECLARE_OBJECT(Sandbox::NetworkRequestBase, Sandbox::meta::object)
SB_META_DECLARE_OBJECT(Sandbox::NetworkRequest, Sandbox::NetworkRequestBase)
SB_META_DECLARE_OBJECT(Sandbox::NetworkDataRequest, Sandbox::NetworkRequestBase)
SB_META_DECLARE_OBJECT(Sandbox::NetworkFileRequest, Sandbox::NetworkRequestBase)
#ifndef SB_NO_RESOURCES
SB_META_DECLARE_OBJECT(Sandbox::ImageRequest, Sandbox::NetworkDataRequest)
#endif
SB_META_DECLARE_OBJECT(Sandbox::NetworkPostData, Sandbox::meta::object)
SB_META_DECLARE_OBJECT(Sandbox::NetworkMultipartFormData, Sandbox::NetworkPostData)
SB_META_DECLARE_OBJECT(Sandbox::NetworkMultipartFormStream, Sandbox::meta::object)
namespace Sandbox {
    
    static const char* MODULE = "sb_net";

    NetworkRequestBase::NetworkRequestBase(const sb::string& url) : m_url(url),m_completed(false),m_error(false),m_received_size(0) {
        m_status_code = 0;
    }
    NetworkRequestBase::~NetworkRequestBase() {
        
    }
    void GHL_CALL NetworkRequestBase::AddRef() const {
        sb_ensure_main_thread();
        add_ref();
    }
    /// release reference
    void GHL_CALL NetworkRequestBase::Release() const {
        sb_ensure_main_thread();
        remove_ref();
    }
    /// ghl interface
    /// url
    const char* GHL_CALL NetworkRequestBase::GetURL() const {
        return m_url.c_str();
    }
    /// headers
    GHL::UInt32 GHL_CALL NetworkRequestBase::GetHeadersCount() const {
        return GHL::UInt32(m_headers.size());
    }
    const char* GHL_CALL NetworkRequestBase::GetHeaderName(GHL::UInt32 idx) const {
        headers_map_t::const_iterator it = m_headers.begin();
        std::advance(it, idx);
        return it->first.c_str();
    }
    const char* GHL_CALL NetworkRequestBase::GetHeaderValue(GHL::UInt32 idx) const {
        headers_map_t::const_iterator it = m_headers.begin();
        std::advance(it, idx);
        return it->second.c_str();
    }
    
    /// received response
    void GHL_CALL NetworkRequestBase::OnResponse(GHL::UInt32 status) {
        m_status_code = status;
        //SB_LOGI("Response " << status);
    }
    /// received header
    void GHL_CALL NetworkRequestBase::OnHeader(const char* name,const char* value) {
        
        m_received_headers[name]=value;
    }
    /// received data
    void GHL_CALL NetworkRequestBase::OnData(const GHL::Byte* data,GHL::UInt32 size) {
        m_received_size += size;
    }
    /// received complete
    void GHL_CALL NetworkRequestBase::OnComplete() {
        sb_ensure_main_thread();
        m_completed = true;
    }
    /// received error
    void GHL_CALL NetworkRequestBase::OnError(const char* error) {
        sb_ensure_main_thread();
        m_completed = true;
        m_error = true;
        if (error) {
            m_error_text = error;
        }
    }
    
    
    /// received data
    void GHL_CALL NetworkRequest::OnData(const GHL::Byte* data,GHL::UInt32 size) {
        NetworkRequestBase::OnData(data, size);
		//LogDebug() << "received data: " << sb::string(reinterpret_cast<const char*>(data), size);
        m_data.append(reinterpret_cast<const char*>(data),size);
    }
    
    NetworkDataRequest::~NetworkDataRequest() {
        ReleaseData();
    }
    
    void NetworkDataRequest::OnData(const GHL::Byte *data, GHL::UInt32 size) {
        NetworkRequestBase::OnData(data, size);
        if (!m_data) {
            m_data = new VectorData<GHL::Byte>();
        }
        size_t pos = m_data->vector().size();
        m_data->vector().resize(pos+size);
        ::memcpy(&m_data->vector()[pos], data, size);
    }
    
    void NetworkDataRequest::ReleaseData() {
        if (m_data) {
            m_data->Release();
            m_data = 0;
        }
    }
#ifndef SB_NO_RESOURCES
    void GHL_CALL ImageRequest::OnComplete() {
        NetworkDataRequest::OnComplete();
        const GHL::Data* data = GetData();
        if (data) {
            m_img = m_resources->CreateImageFromData(data);
        }
        ReleaseData();
    }
#endif
    
    
    
    NetworkFileRequest::NetworkFileRequest(const sb::string& url, GHL::WriteStream* ws) : NetworkRequestBase(url),m_ds(ws) {
        m_ds->AddRef();
    }
    
    NetworkFileRequest::~NetworkFileRequest() {
        ReleaseData();
    }
    
    void GHL_CALL NetworkFileRequest::OnComplete() {
        NetworkRequestBase::OnComplete();
        ReleaseData();
    }
    
    void NetworkFileRequest::ReleaseData() {
        if (m_ds) {
            m_ds->Flush();
            m_ds->Close();
            m_ds->Release();
            m_ds = 0;
        }
    }
    
    void NetworkFileRequest::OnData(const GHL::Byte *data, GHL::UInt32 size) {
        NetworkRequestBase::OnData(data, size);
        if (m_ds) {
            m_ds->Write(data, size);
        }
    }
    
    Network::Network(Resources* res) : m_resources(res) {
        m_net = GHL_CreateNetwork();
    }
    
    Network::~Network() {
        GHL_DestroyNetwork(m_net);
    }
    
    NetworkRequestPtr Network::SimpleGET(const sb::string& url) {
        if (!m_net) return NetworkRequestPtr();
        NetworkRequestPtr request(new NetworkRequest(url));
        ApplyCookie(request);
        if (!m_net->Get(request.get()))
            return NetworkRequestPtr();
        return request;
    }
#ifndef SB_NO_RESOURCES
    ImageRequestPtr Network::GETImage(const sb::string& url) {
        if (!m_net || !m_resources) return ImageRequestPtr();
        ImageRequestPtr request(new ImageRequest(url,m_resources));
        if (!m_net->Get(request.get()))
            return ImageRequestPtr();
        return request;
    }
#endif
    
    NetworkFileRequestPtr Network::GETFile(const sb::string& url,const sb::string& filename) {
        if (!m_net) return NetworkFileRequestPtr();
        GHL::WriteStream* ds = m_resources->OpenWrite(filename.c_str(),true);
        if (!ds) {
            return NetworkFileRequestPtr();
        }
        NetworkFileRequestPtr request(new NetworkFileRequest(url,ds));
        ApplyCookie(request);
        if (!m_net->Get(request.get()))
            return NetworkFileRequestPtr();
        return request;
    }
    NetworkRequestPtr Network::SimplePOST(const sb::string& url, const sb::string& data) {
        if (!m_net) return NetworkRequestPtr();
        NetworkRequestPtr request(new NetworkRequest(url));
        ApplyCookie(request);
        GHL::Data* data_p = GHL_HoldData(reinterpret_cast<const GHL::Byte*>(data.c_str()), GHL::UInt32(data.length()));
        if (!m_net->Post(request.get(),data_p)) {
            if (!request->GetErrorText().empty()) {
                SB_LOGE("network POST error: " << request->GetErrorText());
            }
            data_p->Release();
            return request->GetCompleted() ? request : NetworkRequestPtr();
        }
        data_p->Release();
        return request;
    }
    
    NetworkPostData::NetworkPostData() : m_data(new VectorData<GHL::Byte>()) {
        
    }
    NetworkPostData::~NetworkPostData() {
        m_data->Release();
    }
    
    static const char* rn = "\r\n";
    static const char* dashes = "--";
    
    NetworkMultipartFormData::NetworkMultipartFormData() {
        m_boundary = "0-------------sandbox-multipart-form-data-----123456789";
    }
    
    NetworkMultipartFormData::~NetworkMultipartFormData() {
        
    }
    
    void NetworkMultipartFormData::write_string(const char* str) {
        size_t pos = m_data->vector().size();
        size_t len = ::strlen(str);
        m_data->vector().resize(pos+len,0);
        ::memcpy(&m_data->vector()[pos],str,len);
    }
    
    void NetworkMultipartFormData::write_boundary() {
        write_string(dashes);
        write_string(m_boundary.c_str());
        write_string(rn);
    }
    
    void NetworkMultipartFormData::AddFormField(const sb::string& field, const sb::string& data) {
        write_boundary();
        write_string("Content-Disposition: form-data");
        write_string("; name=\"");
        write_string(field.c_str());
        write_string("\"");
        write_string(rn);
        write_string(rn);
        write_string(data.c_str());
        write_string(rn);
    }
    
    void NetworkMultipartFormData::AddFile(const sb::string& name,
                 const sb::string& filename,
                 const sb::string& content_type,
                 const BinaryDataPtr& content) {
        write_boundary();
        if (!content_type.empty()) {
            write_string("Content-Type: ");
            write_string(content_type.c_str());
            write_string(rn);
        }
        write_string("Content-Disposition: form-data");
        if (!name.empty()) {
            write_string("; name=\"");
            write_string(name.c_str());
            write_string("\"");
        }
        if (!filename.empty()) {
            write_string("; filename=\"");
            write_string(filename.c_str());
            write_string("\"");
        }
        write_string(rn);
        GHL::Data* data = 0;
        if (content) {
            data = content->GetData();
        }
        if (data) {
            write_string(rn);
            size_t pos = m_data->vector().size();
            m_data->vector().resize(pos+data->GetSize());
            ::memcpy(&m_data->vector()[pos], data->GetData(), data->GetSize());
        }
        write_string(rn);
    }
    
    void NetworkMultipartFormData::Close() {
        write_string(dashes);
        write_string(m_boundary.c_str());
        write_string(dashes);
    }
    
    void NetworkMultipartFormData::Setup(NetworkRequestBase *request) {
        request->SetHeader("Content-Type", "multipart/form-data; boundary=" + m_boundary);
    }
    
    
    NetworkMultipartFormStream::NetworkMultipartFormStream() {
        m_boundary.assign("0-------------sandbox-multipart-form-data-stream----123456789");
        
    }
    void NetworkMultipartFormStream::Setup(NetworkRequestBase *request) {
        request->SetHeader("Content-Type", "multipart/form-data; boundary=" + m_boundary);
//        char buf[32];
//        sb::snprintf(buf, sizeof(buf), "%d",GetTotalSize());
//        request->SetHeader("Content-Length", buf);
    }
    
    void NetworkMultipartFormStream::write_boundary() {
        AddString(dashes);
        AddString(m_boundary);
        AddString(rn);
    }

    void NetworkMultipartFormStream::AddFormField(const sb::string& field, const sb::string& data) {
        write_boundary();
        AddString("Content-Disposition: form-data; name=\"");
        AddString(field);
        AddString("\"");
        AddString(rn);
        AddString(rn);
        AddString(data);
        AddString(rn);
    }
    
    void NetworkMultipartFormStream::AddFormField(const sb::string& name,
                                                  const sb::string& filename,
                                                  const sb::string& content_type,
                                                  const sb::string& data) {
        write_boundary();
        if (!content_type.empty()) {
            AddString("Content-Type: ");
            AddString(content_type);
            AddString(rn);
        }
        AddString("Content-Disposition: form-data");
        if (!name.empty()) {
            AddString("; name=\"");
            AddString(name);
            AddString("\"");
        }
        if (!filename.empty()) {
            AddString("; filename=\"");
            AddString(filename);
            AddString("\"");
        }
        AddString(rn);
        AddString(rn);
        AddString(data);
        AddString(rn);
    }
    
    void NetworkMultipartFormStream::AddStream(const sb::string& name,
                   const sb::string& filename,
                   const sb::string& content_type,
                                               GHL::DataStream* data) {
        
        write_boundary();
        if (!content_type.empty()) {
            AddString("Content-Type: ");
            AddString(content_type);
            AddString(rn);
        }
        AddString("Content-Disposition: form-data");
        if (!name.empty()) {
            AddString("; name=\"");
            AddString(name);
            AddString("\"");
        }
        if (!filename.empty()) {
            AddString("; filename=\"");
            AddString(filename);
            AddString("\"");
        }
        AddString(rn);
        AddString(rn);
        MultiStream::AddStream(data);
        AddString(rn);
    }
    
    
    void NetworkMultipartFormStream::Close() {
        AddString(dashes);
        AddString(m_boundary);
        AddString(dashes);
    }
       
    
    NetworkRequestPtr Network::POST(const sb::string& url, const NetworkPostDataPtr& data) {
        if (!m_net) return NetworkRequestPtr();
        NetworkRequestPtr request(new NetworkRequest(url));
        data->Setup(request.get());
        ApplyCookie(request);
        GHL::Data* data_p = data->GetData();
        if (!m_net->Post(request.get(),data_p)) {
            if (!request->GetErrorText().empty()) {
                SB_LOGE( "network POST error: " << request->GetErrorText() );
            }
            return request->GetCompleted() ? request : NetworkRequestPtr();
        }
        return request;
    }
    
    NetworkRequestPtr Network::POSTFormStream(const sb::string& url,
                                 const NetworkMultipartFormStreamPtr& data) {
        if (!m_net) return NetworkRequestPtr();
        NetworkRequestPtr request(new NetworkRequest(url));
        data->Setup(request.get());
        ApplyCookie(request);
        if (!m_net->PostStream(request.get(),data.get())) {
            if (!request->GetErrorText().empty()) {
                SB_LOGE( "network POST error: " << request->GetErrorText() );
            }
            return request->GetCompleted() ? request : NetworkRequestPtr();
        }
        return request;
    }
    
    void Network::ApplyCookie(const NetworkRequestBaseBtr& req) {
        const char* url = req->GetURL();
        for (sb::map<sb::string, sb::string>::const_iterator it = m_cookie.begin();it!=m_cookie.end();++it) {
            if (strncmp(url, it->first.c_str(), it->first.length())==0) {
                req->SetHeader("Cookie", it->second);
            }
        }
    }

    void Network::Process() {
        if (m_net) m_net->Process();
    }
    
}
