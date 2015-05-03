// Federate.h: interface for the Federate class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FEDERATE_H
#define FEDERATE_H

#include <Types.h>
#include <Message.h>

#include <vector>

class Federate  
{
public:
    Federate() { m_Dimension = 1; }

    void SetName(std::string name) { m_Name = name; }
    void SetDimension(uint dim) { m_Dimension = dim; }
    void AddMessage(std::string name, Message* m)
    { m_MessageMap[name] = m; }
    void SetSubscriptionMap(MessageMapType map)
    { m_SubscriptionMap = map; }
    
    std::string Name() { return m_Name; }
    uint Dimension() const { return m_Dimension; }
    uint Dimension(uint dim_num) const;

    Message* FindMessage(std::string name) const;
    const MessageMapType& MessageMap() {return m_MessageMap; }
    const MessageMapType& SubscriptionMap() {return m_SubscriptionMap; }

    void GenerateCppCode(FILE *iov, bool gen_send_code) const;
    void OutputOutgoingMsgInfo(FILE *iov, int index);
    void OutputFederateInfo(FILE *iov, int index);
    void GenerateFederateData(int index);

    static void Initialize();

private:
    std::string m_Name;
    MessageMapType m_MessageMap, m_SubscriptionMap;
    uint m_Dimension;
};

#endif
