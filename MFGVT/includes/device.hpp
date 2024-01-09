/*
                    GNU GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.
*/


#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include "../includes/container.hpp"

#include <future>
#include <mutex>


/// @brief class ui definit un materiel contenant des conteneur
class Device  : public utilitys::MainPathSharedTemplate
{
    public:
        Device( const std::string & _name);
        Device( const Device & _lref);
        Device( const Device && _rref);
        Device & operator=(const Device & _lref);
        Device & operator=(const Device && _rref);

        std::vector<VContainer> & get_containers(void);
        const std::string get_name(void)const;
        void update(   std::vector< File > & _files,bool _forceUpdate = false , unsigned int _n_thread = 0);

        typedef std::shared_ptr< std::vector<Device> > ptrVecDevice;

        static std::tuple<std::string, ptrVecDevice > load(const std::string & _ressourcePath);
    protected:

        std::vector<VContainer> m_containers;
        std::string m_name;

        bool m_update;

    private :

        std::mutex m_mutex;

        void findDevice_th( const std::vector<File>::iterator & _beg , const std::vector<File>::iterator & _end,  std::vector<File> & _out );
        void container_init_th(  const std::vector<VContainer>::iterator & _beg , const std::vector<VContainer>::iterator & _end , const std::vector<File> & pathDevice );

        void add2Container( File const & _in , VContainer & _out );

};

typedef std::shared_ptr<std::vector<Device>> ptrVecDevice;


/// @brief initialise avec un nom
/// @param _name 
Device::Device( const std::string & _name):m_name(_name),m_update(false)
{
}

/// @brief operateur de copy
/// @param _lref 
Device & Device::operator=(const Device & _lref)
{
    this->m_containers = _lref.m_containers;
    this->m_name =  _lref.m_name;
    this->m_update = _lref.m_update;

    return *this;
}

/// @brief operateur de copy
/// @param _lref 
Device & Device::operator=(const Device && _rref)
{
    this->m_containers = _rref.m_containers;
    this->m_name =  _rref.m_name;
    this->m_update = _rref.m_update;

    return *this;
}

/// @brief constructeur de copy
/// @param _lref 
Device::Device( const Device & _lref)
{
    *this = _lref;
}

/// @brief constructeur de copy
/// @param _lref 
Device::Device( const Device && _rref)
{
    *this = _rref;
}


/// @brief retourn le conteneur
/// @return std::vector<Container>
std::vector<VContainer> & Device::get_containers(void)
{
    return this->m_containers;
}

/// @brief retourn le nom du materiel
/// @return str
const std::string Device::get_name(void)const
{
    return this->m_name;
}

/// @brief 
/// @param _beg debut de list d'entré
/// @param _end fin de liste d'entré
/// @param _out list de sous chemin chemin
void Device::findDevice_th( const std::vector<File>::iterator & _beg , const std::vector<File>::iterator & _end,  std::vector<File> & _out )
{
    for(auto f = _beg ; f != _end ; f++)
    {
        
        auto tmp = File::sep_sub_and_name( f->get_file() );
        
        bool found =false;
        //! ici on cherche u materiel
        
        while ( std::get<0>(tmp).size() > 0)
        {
         tmp = File::sep_sub_and_name( std::get<0>(tmp) );

            std::string tmpName = std::get<1>(tmp);
            utilitys::upper(tmpName);
            
            if( tmpName == this->m_name )
            {
                found = true;
                break;
            }
        }
        
        if( found )
        {
            this->m_mutex.lock();

            _out.emplace_back( f->get_file() );
            _out.back().get_ptr_lnk() = f->get_ptr_lnk();

            this->m_mutex.unlock();
        }
           
    }
}


/// @brief ajoute au groupe la version a qui est de meme contenair que attendu
///la fonction appel par recursivité si un lien symbolique est trouver et que c'est un repertoir
void Device::add2Container( File const & _in , VContainer & _out )
{
    ///lien d'un repertoir , recurcivité sour les enfant
    if( _in.get_lnk()->size() > 1 )
    {
        for( auto & sym : *_in.get_lnk())
            this->add2Container( sym , _out );
        

        return ;
    }

    Version tmp = static_cast<Version>(_in);

    tmp.device = this->m_name;
    
    tmp.id = utilitys::regSearch( _out.get_reg_id() , tmp.get_name());
    
    if(tmp.id.size() == 0)
        tmp.id = tmp.get_name();

    tmp.autor =  std::move(utilitys::regSearch( _out.get_reg_autor() , tmp.get_name()));
    tmp.part =  std::move(utilitys::regSearch( _out.get_reg_part() , tmp.get_name()));
    tmp.version =  std::move(utilitys::regSearch( _out.get_reg_version() , tmp.get_name()));

    tmp.update_dates();
    
    if( _in.is_symLnk() )
    {
        tmp.createLnkVersion();

        if( tmp.is_lnk() )
        {
            tmp.ptr_lnk->id = utilitys::regSearch( _out.get_reg_id() , tmp.ptr_lnk->get_name());
            
            if(tmp.ptr_lnk->id.size() == 0)
                tmp.ptr_lnk->id = tmp.ptr_lnk->get_name();

            tmp.ptr_lnk->autor =  std::move(utilitys::regSearch( _out.get_reg_autor() , tmp.ptr_lnk->get_name()));
            tmp.ptr_lnk->part =  std::move(utilitys::regSearch( _out.get_reg_part() , tmp.ptr_lnk->get_name()));
            tmp.ptr_lnk->version =  std::move(utilitys::regSearch( _out.get_reg_version() , tmp.ptr_lnk->get_name()));

            tmp.ptr_lnk->update_dates();

            // lien de lien genere une erreur
            if( tmp.ptr_lnk->is_lnk())
                tmp.add_error(Version::EF_LNK);
        }
    }

    _out.add2GrpVersion( std::move(tmp) );
}

/// @brief met a jour les conteneur fonction threadable
void Device::container_init_th(  const std::vector< VContainer >::iterator & _beg , const std::vector< VContainer >::iterator & _end , const std::vector< File > & pathDevice)
{
    for(auto  c = _beg ; c != _end ; c++)
    {
        c->clear();

        for(const auto &  pd : pathDevice)
        {
            std::string tmpStr = pd.get_file();

            utilitys::upper(tmpStr);
            auto found = tmpStr.find( c->get_name() );

            //ajoute si l'extension est supporté et si le conteneur a ete trouvé

            if( found!=std::string::npos) 
                if( c->get_authExt().find( pd.get_ext() ) != std::string::npos )
                    this->add2Container( pd , *c );
            
        } 
    }
}

/// @brief met a jour les conteneur en scanant une racine pour y trouver le materiel et les conteneur
/// @param _mainPath racine a scanné
/// @param _files  tout les chemin fichier compatible
/// @param _forceUpdate force la mise a jour si deja scanné
void Device::update(  std::vector< File> & _files , bool _forceUpdate ,unsigned int _n_thread)
{

    if( this->m_update && !_forceUpdate)
        return ;
        
    std::vector< File > pathDevice ;

    utilitys::multi_thread_callback( std::bind(&Device::findDevice_th, this, std::placeholders::_1 , std::placeholders::_2 , std::ref(pathDevice)) , _files , _n_thread );

    utilitys::multi_thread_callback( std::bind(&Device::container_init_th, this, std::placeholders::_1 , std::placeholders::_2 , std::ref(pathDevice)) , this->m_containers , _n_thread );

    this->m_update  = true;
}


/// @brief charge la repertoir de travail et un poiteur de la liste des materiel
/// @param _ressourcePath repertoir des resosurces
/// @return tuple<str , ptrVecDevice>
std::tuple<std::string,ptrVecDevice> Device::load(const std::string & _ressourcePath)
{
    const auto start{std::chrono::steady_clock::now()};
    ptrVecDevice devices;


    //charge les fichier de maniere asynchrone // utlise 3 thread
    std::future< std::vector<std::string> > deviceTh = std::async ( utilitys::readList, _ressourcePath + "\\devicesList.csv" , true);
    std::future< std::vector<std::string> > contTh = std::async ( utilitys::readList, _ressourcePath + "\\containerList.csv" , true);
    std::future< std::vector<std::string> > parameterTh = std::async (utilitys::readList, _ressourcePath + "\\parameters.csv" , false);
    
    std::vector<std::string>&& lsContainer = contTh.get();
    std::vector<std::string>&& lsParameter = parameterTh.get();
    std::vector<std::string>&& lsDevices = deviceTh.get();


    if(  lsContainer.size() == 0)
        throw std::logic_error("no containers found");
    
    if( lsDevices.size() == 0 )
        throw std::logic_error("no devices subpath found");
    
    if(lsParameter.size() == 0)
        throw std::logic_error("no parametres found");


    std::string && mainPath =  utilitys::getParamValue("WORK_DIR" , lsParameter , true);

    if(mainPath.size() == 0)
        throw std::logic_error("no mainPath  found");

    if(mainPath.back() == '\\' || mainPath.back() == '/')
        mainPath.pop_back();


    devices = ptrVecDevice(new std::vector<Device>());

    //contruis une liste de contenaire patron pour ne pas repeter getParamValue
    std::vector<VContainer> tmpCont;
    for(const auto & cnt : lsContainer)
    {
        tmpCont.emplace_back( cnt );
        tmpCont.back().set_authExt( std::move(utilitys::getParamValue("AUTHEXT[" + cnt +"]" , lsParameter , true)) );

        tmpCont.back().set_reg_version( std::move(utilitys::getParamValue("VERSIONREGEX[" + cnt +"]" , lsParameter , true)));
        tmpCont.back().set_reg_id( std::move(utilitys::getParamValue("IDREGEX[" + cnt +"]" , lsParameter , true)));
        tmpCont.back().set_reg_autor( std::move(utilitys::getParamValue("AUTORREGEX[" + cnt +"]" , lsParameter , true)));
        tmpCont.back().set_reg_part( std::move(utilitys::getParamValue("PARTREGEX[" + cnt +"]" , lsParameter , true)));
    }

    //construit la liste de materiel
    for(const auto & name : lsDevices)
    {
        //cherche si pas deja inlcu

        bool found = false;
        for( const auto & dev : *devices)
            if(name == dev.get_name())
                found = true;

        //si le cas passe au suivant
        if( found)
            continue;

        //ajoute un materiel
        devices->emplace_back( name );

        //ajoute les contenaire au materiel
        for(const auto & cnt : tmpCont)
        {
            devices->back().get_containers().emplace_back( std::move(cnt.get_name()) );
            devices->back().get_containers().back().set_authExt( std::move(cnt.get_authExt()));

            devices->back().get_containers().back().set_reg_version(  std::move(cnt.get_reg_version()) );
            devices->back().get_containers().back().set_reg_id(  std::move(cnt.get_reg_id()) );
            devices->back().get_containers().back().set_reg_autor(  std::move(cnt.get_reg_autor()) );
            devices->back().get_containers().back().set_reg_part(  std::move(cnt.get_reg_part()) );
        }
    }

    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};

    return {mainPath , devices};
}

#endif