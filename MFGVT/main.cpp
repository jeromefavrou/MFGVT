#include <format>
#include <windows.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <tuple>
#include <filesystem>
#include <chrono>
#include <future>
#include <utility>
#include <thread>
#include <regex>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <locale>

#include <gtkmm-3.0/gtkmm.h>

       // "C:/msys64/ucrt64/include/**"
       // "C:/msys64/ucrt64/lib/**"
std::wstring convert_string_to_wstring(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}
std::string convert_wstring_to_string(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

void cpyToPP(std::string const & _cpy)
{
     // Chaîne de caractères à mettre dans le presse-papiers
    const char* texte = "Exemple de texte dans le presse-papiers.";

    // Ouvrir le presse-papiers
    if (OpenClipboard(NULL)) {
        // Vider le presse-papiers
        EmptyClipboard();

        // Allouer de la mémoire pour la chaîne de caractères
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, strlen(_cpy.c_str()) + 1);
        if (hMem != NULL) {
            // Obtenir un pointeur vers la mémoire allouée
            char* pMem = static_cast<char*>(GlobalLock(hMem));
            if (pMem != NULL) {
                // Copier la chaîne de caractères dans la mémoire allouée
                strcpy(pMem, _cpy.c_str());

                // Libérer la mémoire
                GlobalUnlock(hMem);

                // Mettre la mémoire dans le presse-papiers
                SetClipboardData(CF_TEXT, hMem);

                // Fermer le presse-papiers
                CloseClipboard();

            } 
            else
             {
                std::cerr << "Erreur lors du verrouillage de la mémoire globale." << std::endl;
            }
        } 
        else {
            std::cerr << "Erreur lors de l'allocation de la mémoire globale." << std::endl;
        }
    } 
    else {
        std::cerr << "Erreur lors de l'ouverture du presse-papiers." << std::endl;
    }

}

std::string get_createDate(std::string const & _file)
{
    // Obtenir le handle du fichier
    HANDLE hFile = CreateFile(_file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Erreur lors de l'ouverture du fichier." << std::endl;
        return "";
    }

    // Obtenir les informations sur le fichier, y compris la date de création
    FILETIME creationTime, lastAccessTime, lastWriteTime;
    if (GetFileTime(hFile, &creationTime, &lastAccessTime, &lastWriteTime)) {
        // Convertir FILETIME en système local time
        SYSTEMTIME sysCreationTime;
        if (FileTimeToSystemTime(&creationTime, &sysCreationTime)) {
            // Afficher la date de création

            std::stringstream ss;
            ss << (sysCreationTime.wYear < 10 ? "0" : "")<< sysCreationTime.wYear << "-" <<  (sysCreationTime.wMonth < 10 ? "0" : "") << sysCreationTime.wMonth << "-" << (sysCreationTime.wDay < 10 ? "0" : "") << sysCreationTime.wDay << std::endl;
            
            std::string tmp;
            std::getline(ss , tmp);
            return tmp;
        } else {
            return "";
        }
    } else {
        return "";
    }

    // Fermer le handle du fichier
    CloseHandle(hFile);

    return "";
}


//REMOVE CHILDRENS

void gtkmmRemoveChilds(Gtk::Container & _wref)
{
        for (auto& child : _wref.get_children()) 
            _wref.remove(*child);
}

//uppper
void upper(std::string & _refstr)
{
    for(std::string::iterator it = _refstr.begin() ; it != _refstr.end() ; it++ )
        if(*it >= 0x61 && *it <= 0x7A)
            *it = *it - char(32) ;
}

void upperList(std::vector<std::string > & _vecstrref)
{
    for(auto it = _vecstrref.begin() ; it != _vecstrref.end() ; it++ )
        upper(*it);
}
//separe un path en sous path et nom
std::tuple<std::string , std::string> sep_sub_and_name(std::string const & _str)
{
    int idx = 0;

    for(auto i = _str.size() ; i >= 0 ; i--)
    {
        if(_str[i] == '\\' || _str[i] == '/' )
        {
            idx = i ; 
            break;
        }
    }

    if( idx <= 0)
        return {"" , _str};

    return {_str.substr(0 , idx) , _str.substr(idx+1 , _str.size())};
}

//separe un nom en sous nom et extension
std::tuple<std::string , std::string> sep_name_and_ext(std::string const & _str)
{
    int idx = 0;

    for(auto i = _str.size() ; i >= 0 ; i--)
    {
        if(_str[i] == '.' )
        {
            idx = i ; 
            break;
        }
    }

    if( idx <= 0)
        return {_str , ""};

    return {_str.substr(0 , idx) , _str.substr(idx+1 , _str.size())};
}

template<char _c> std::tuple<std::string , std::string> sep_string(std::string const & _str)
{
    int idx = 0;

    for(auto i = 0 ; i<_str.size() ; i++)
    {
        if(_str[i] == _c )
        {
            idx = i ; 
            break;
        }
    }

    return {_str.substr(0 , idx) , _str.substr(idx+1 , _str.size())};
}

std::vector<std::string> listerFichiers(const std::string& dossier) 
{
    std::vector<std::string> res;
    try 
    {
        for (const auto& entry : std::filesystem::directory_iterator(dossier))
        {
            if (std::filesystem::is_regular_file(entry.status())) 
            {
                res.push_back( entry.path().filename().string() );
            }
        }
    } 
    catch (const std::exception& e)
    {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }

        return res;
}

std::vector<std::string> listerDossier(const std::string& dossier)
{

    std::vector<std::string> res;
    try 
    {
        for (const auto& entry : std::filesystem::directory_iterator(dossier))
        {
            if (std::filesystem::is_directory(entry.status())) 
            {
                res.push_back( entry.path().filename().string() );
            }
        }
    } 
    catch (const std::exception& e)
    {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }

        return res;
}

void listerFichiers(const std::filesystem::path& chemin, std::vector<std::string>& fichiers) {
    try {
        for (const auto& entry : std::filesystem::directory_iterator(chemin)) {
            if (std::filesystem::is_directory(entry.status())) {
                // Appel récursif pour traiter les sous-répertoires
                listerFichiers(entry.path(), fichiers);
            } else if (std::filesystem::is_regular_file(entry.status())) {
                // Ajouter le fichier au vecteur
                fichiers.push_back(chemin.string() + "\\"+ entry.path().filename().string());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }
}

void writeList(const std::string & _filePath , const std::vector<std::string> & _files)
{
    std::ofstream f( _filePath.c_str() );

    for (const auto& file : _files) 
        f << file << std::endl;
}


//lit l'enseble des ligne d'un fichier et stock dans un vecteur
static std::vector<std::string>  readList(const std::string && _filePath , bool _upper)
{
    std::ifstream f;
    //actives les exception sur badbit
    f.exceptions( std::ifstream::badbit);
    
    std::vector<std::string> res;
    res.reserve(100);
    try
    {
        f.open( _filePath );

        if( !f.good())
            throw std::runtime_error("Erreur lors de l'ouverture du fichier : " + _filePath);

        std::string tmp;
        
        while (std::getline( f , tmp)) 
            res.push_back( std::move(tmp) );
    }
    catch (const std::ifstream::failure& e) 
    {
        std::cerr  << e.what() << std::endl;
    }

    res.shrink_to_fit();

    if( _upper)
            upperList(res);

    return  res;
}

class Version
{
    public : 
        Version(void);
        Version(const Version & _cpy);

        const bool cmpFile(const Version & _cpy) const ;


        std::string name ,dir ,id , creatDate , modifDate, autor , comment , extension ,version , part; 
    protected :

    
};

Version::Version(void)
{
    
}


Version::Version(const Version & _cpy)
{
    this->dir=_cpy.dir;
    this->name=_cpy.name;
    this->id=_cpy.id;
    this->creatDate=_cpy.creatDate;
    this->autor=_cpy.autor;
    this->comment=_cpy.comment; 
    this->extension =_cpy.extension;
    this->version =_cpy.version;
    this->modifDate = _cpy.modifDate;
    this->part=_cpy.part;
}

const bool Version::cmpFile(const Version & _cpy) const 
{
    return _cpy.dir == this->dir && _cpy.name == this->name && _cpy.extension == this->extension ;
}

class GrpVersion : public std::vector<Version>
{
    public:

    const std::string get_name(void) const;
    const std::string get_id(void) const;
    const std::string get_autor(void) const;
    const std::string get_createDate(void) const;
    const std::string get_modifDate(void) const;

    void merge(const GrpVersion & _grp);
    
};


const std::string GrpVersion::get_name(void) const
{
    return this->front().name;
}

const std::string GrpVersion::get_id(void) const
{
    return this->front().id;
}

const std::string GrpVersion::get_autor(void) const
{
    return this->front().autor;
}

const std::string GrpVersion::get_createDate(void) const
{
    return this->front().creatDate;
}

const std::string GrpVersion::get_modifDate(void) const
{
    return this->front().modifDate;
}

void GrpVersion::merge(const GrpVersion & _grp)
{
    for(const auto &  vers1 : _grp)
    {

        bool found = false;
        for(const auto &  vers2 : *this)
        {
            if( vers1.cmpFile(vers2) )
            {
                found = true;
                break;
            }
        }

        if( !found )
        {
            this->push_back(vers1);
        }
    }
}



class Device ;
typedef std::shared_ptr<std::vector<Device>> ptrVecDevice;
class Container ;
class TemplateGui
{
   public:
      TemplateGui(void);

      virtual void addCssProvider(Glib::RefPtr<Gtk::CssProvider> _cssProvider);
      virtual void addParent(Gtk::Window* _parent);
      virtual void addDevice(ptrVecDevice _devices);
      virtual void addMainPath(std::shared_ptr<const std::string> _mainPath);

      Glib::RefPtr<Gtk::CssProvider> const getCssProvider(void);
      Gtk::Window* const getParent(void);
      ptrVecDevice const getDevice(void);
      std::shared_ptr<const std::string> const getMainPath(void);

      Glib::RefPtr<Gtk::CssProvider> const atCssProvider(void);
      Gtk::Window* const atParent(void);
      ptrVecDevice const atDevice(void);
      std::shared_ptr<const std::string> const atMainPath(void);


      virtual ~TemplateGui(void);
      
      private :
protected :
      Gtk::Window* m_parent;

      Glib::RefPtr<Gtk::CssProvider> m_cssProvider;

      ptrVecDevice m_devices;

      std::shared_ptr<const std::string> m_mainPath;
};

TemplateGui::TemplateGui(void)
{
    this->m_parent = nullptr;
}

TemplateGui::~TemplateGui(void)
{
    this->m_parent = nullptr;
}

//setter
void TemplateGui::addCssProvider(Glib::RefPtr<Gtk::CssProvider> _cssProvider)
{
    this->m_cssProvider = _cssProvider;
}

//setter
void TemplateGui::addDevice( ptrVecDevice _devices )
{
    this->m_devices = _devices;
}

//setter
void TemplateGui::addParent(Gtk::Window* _parent)
{
    this->m_parent  = _parent;
}

//setter
void TemplateGui::addMainPath(std::shared_ptr<const std::string> _mainPath)
{
    this->m_mainPath  = _mainPath;
}

//getter
Glib::RefPtr<Gtk::CssProvider> const TemplateGui::getCssProvider(void)
{
    return this->m_cssProvider;
}

//getter
Gtk::Window* const TemplateGui::getParent(void)
{
    return this->m_parent;
}

ptrVecDevice const TemplateGui::getDevice(void)
{
    return this->m_devices;
}

std::shared_ptr<const std::string> const TemplateGui::getMainPath(void)
{
    return this->m_mainPath;
}

//acces securisé 
std::shared_ptr<const std::string> const TemplateGui::atMainPath(void)
{
    if(!this->m_mainPath)
        throw std::logic_error("bad devices acces");

    return this->m_mainPath;
}
//acces securisé 
ptrVecDevice const TemplateGui::atDevice(void)
{
    if(!this->m_devices)
        throw std::logic_error("bad devices acces");

    return this->m_devices;
}
//acces securisé 
Glib::RefPtr<Gtk::CssProvider> const TemplateGui::atCssProvider(void)
{
    if(!this->m_cssProvider)
        throw std::logic_error("bad provider acces");

    return this->m_cssProvider;
}

//acces securisé 
Gtk::Window* const TemplateGui::atParent(void)
{
    if(this->m_parent == nullptr)
       throw std::logic_error("bad raw window acces");

    return this->m_parent;
}

class DialogThread : public Gtk::Dialog
{
   public :
      DialogThread(void);
      ~DialogThread(void);

      void run(void);
      void stop(void);

   private :

      bool thread_func(void);

       std::thread m_th;
       std::atomic<bool> m_continue , m_end ;
       Glib::Dispatcher m_signalDone;
      
};
DialogThread::DialogThread(void):Gtk::Dialog()
{
    this->m_signalDone.connect(sigc::mem_fun(this, &DialogThread::show_all));
    this->m_continue = false;
    this->m_end = false;
}

DialogThread::~DialogThread(void)
{
    this->stop();
}

void DialogThread::run(void)
{
    
    if(this->m_continue || this->m_th.joinable())
        return ;

    this->m_end = false;
    this->m_continue = true;
    this->m_th =  std::thread(&DialogThread::thread_func, this);

    
}

void DialogThread::stop(void)
{
    if(!this->m_continue)
        return;
    
    this->m_continue = false;
    if(this->m_th.joinable())
        this->m_th.join();

}

bool DialogThread::thread_func(void)
{
    while( this->m_continue )
    {
        this->m_signalDone.emit();

        const std::chrono::duration<double, std::milli> duration(500);

        std::this_thread::sleep_for(duration);
    }

    this->m_end = true;
    return true;
}


class VersionShower :  public TemplateGui , public Gtk::VBox , public std::vector<GrpVersion>
{
    public:

        VersionShower(void);
        void update_view( void);

    private :

       void on_clic(GrpVersion _cpy);
       void update_complet_path(void);
       void clipBoardCpy(void);

       Gtk::VBox id , comment , createDate , m_grpInfoPath;
       GrpVersion m_current_sel;
       Gtk::Button m_groupeInfo , m_cpyPath;
       Gtk::Label m_createDate , m_modifDate , m_autor;

       Gtk::ComboBoxText m_version_sel, m_ext_sel ,m_part_sel;
       Gtk::Entry m_complet_path;

       DialogThread m_dialogInfoGrp;

       bool m_onclickLock;
};

VersionShower::VersionShower(void):TemplateGui(),Gtk::VBox()
{
    this->m_onclickLock = false;
    const int colSize = 3;
    Gtk::Frame *tmp[colSize] ;
    Gtk::Table *tab2 = Gtk::manage(new Gtk::Table( 2  , colSize ));  
    
    tab2->attach(*Gtk::manage(new Gtk::Label("id")), 0, 1, 0, 1 , Gtk::SHRINK, Gtk::SHRINK); 
    tab2->attach(*Gtk::manage(new Gtk::Label("commentaire")), 1, 2, 0, 1 , Gtk::SHRINK, Gtk::SHRINK);
    tab2->attach(*Gtk::manage(new Gtk::Label("Date de création")), 2, 3, 0, 1 , Gtk::SHRINK, Gtk::SHRINK);

    for(auto i =0 ; i < colSize ; i++)
    {
        tmp[i] =  Gtk::manage(new Gtk::Frame());
        tab2->attach(*tmp[i], i, i+1, 1, 2 ); 
    }

    tmp[0]->add( this->id );
    tmp[1]->add( this->comment );
    tmp[2]->add( this->createDate);
    
        
    Gtk::ScrolledWindow *sw =Gtk::manage(new Gtk::ScrolledWindow());
    sw->add(*tab2);

    this->pack_start(*sw);


    // parite gestion d'une selection
    Gtk::Table *tab3 = Gtk::manage(new Gtk::Table( 2  , 6 ));
    tab3->attach( *Gtk::manage(new Gtk::Label("")) , 0, 1, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( *Gtk::manage(new Gtk::Label("CompletePath")), 1, 2, 0, 1 ); 
    tab3->attach( *Gtk::manage(new Gtk::Label("P")), 2, 3, 0, 1 , Gtk::SHRINK , Gtk::SHRINK); 
    tab3->attach( *Gtk::manage(new Gtk::Label("V")) , 3, 4, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( *Gtk::manage(new Gtk::Label("E")), 4, 5, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( *Gtk::manage(new Gtk::Label("")), 5, 6, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);

    tab3->attach( this->m_groupeInfo , 0, 1, 1, 2 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( this->m_complet_path, 1, 2, 1, 2 ); 
    tab3->attach( this->m_part_sel, 2, 3, 1, 2 , Gtk::SHRINK , Gtk::SHRINK); 
    tab3->attach( this->m_version_sel , 3, 4, 1, 2 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( this->m_ext_sel, 4, 5, 1, 2 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( this->m_cpyPath, 5, 6, 1, 2 , Gtk::SHRINK , Gtk::SHRINK);

    this->m_complet_path.set_editable(false);

    this->m_groupeInfo.signal_clicked().connect(  sigc::mem_fun(this->m_dialogInfoGrp ,&DialogThread::show_all)  );
    this->m_version_sel.signal_changed().connect(  sigc::mem_fun(*this,&VersionShower::update_complet_path)  );
    this->m_ext_sel.signal_changed().connect(  sigc::mem_fun(*this,&VersionShower::update_complet_path)  );
    this->m_cpyPath.signal_clicked().connect(  sigc::mem_fun(*this,&VersionShower::clipBoardCpy)  );

    

    Gtk::Table *infoBare = Gtk::manage(new Gtk::Table( 2  , 3 ));

    infoBare->attach( *Gtk::manage(new Gtk::Label("creatDate")) , 0, 1, 0, 1 );
    infoBare->attach(  *Gtk::manage(new Gtk::Label("modif date")), 1, 2, 0, 1 ); 
    infoBare->attach(  *Gtk::manage(new Gtk::Label("autor")), 2, 3, 0, 1 ); 

    infoBare->attach(this->m_createDate , 0, 1, 1, 2 );
    infoBare->attach( this->m_modifDate, 1, 2, 1, 2 ); 
    infoBare->attach( this->m_autor, 2, 3, 1, 2 ); 

    this->pack_end(*infoBare , Gtk::PACK_SHRINK);
    
    this->pack_end(*tab3 , Gtk::PACK_SHRINK);


    this->m_dialogInfoGrp.signal_hide().connect(sigc::mem_fun( this->m_dialogInfoGrp, &DialogThread::stop));
    this->m_dialogInfoGrp.signal_show().connect(sigc::mem_fun( this->m_dialogInfoGrp, &DialogThread::run));
    this->m_dialogInfoGrp.set_title( "Grp Info");
    this->m_dialogInfoGrp.set_resizable(true  );
    
    this->m_dialogInfoGrp.set_position(Gtk::WIN_POS_CENTER);
    this->m_dialogInfoGrp.resize(640, 480);


    Gtk::Table *infoGrp = Gtk::manage(new Gtk::Table( 2  , 1 ));

    infoGrp->attach( *Gtk::manage(new Gtk::Label("Path File")) , 0, 1, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);

    infoGrp->attach( this->m_grpInfoPath , 0, 1, 1, 2 ,Gtk::SHRINK , Gtk::SHRINK);

    Gtk::ScrolledWindow *sw2 =Gtk::manage(new Gtk::ScrolledWindow());
    sw2->set_policy(Gtk::PolicyType::POLICY_AUTOMATIC, Gtk::PolicyType::POLICY_AUTOMATIC);
    sw2->add(*infoGrp);
    sw2->set_vexpand(true);
    this->m_dialogInfoGrp.get_content_area()->add( *sw2 );

}
void VersionShower::clipBoardCpy(void)
{
    if(this->m_current_sel.size() == 0)
        return ;

    cpyToPP(  this->m_complet_path.get_text() );

}
void VersionShower::update_view( void)
{

    // fusionne les groupeVersion
    for( auto it = this->begin(); it != this->end() ; it++ )
    {
        if(it +1 <  this->end())
        {
            for( auto it2 = it+1 ; it2 != this->end() ; it2++ )
            {
                if( it->get_id() == it2->get_id() )
                {
                    it->merge(*it2);
                    it=this->begin()-1;
                    this->erase(it2);
                    break;
                }
            } 
        }
    }



    
    gtkmmRemoveChilds(this->id);
    gtkmmRemoveChilds(this->comment);
    gtkmmRemoveChilds(this->createDate);

    for(GrpVersion & grp : *this)
    {
        auto bp = Gtk::manage(new Gtk::Button( grp.get_id().size() > 0 ? grp.get_id() : grp.get_name() , Gtk::PACK_SHRINK ));

        bp->signal_clicked().connect(sigc::bind<GrpVersion>(sigc::mem_fun(*this,&VersionShower::on_clic), grp));
        this->id.pack_start(*bp,Gtk::PACK_SHRINK);


        auto lab = Gtk::manage(new Gtk::Button( "no comment", Gtk::PACK_SHRINK ));

        this->comment.pack_start(*lab,Gtk::PACK_SHRINK);

        auto lab2 = Gtk::manage(new Gtk::Button(grp.get_createDate() , Gtk::PACK_SHRINK ));

        this->createDate.pack_start(*lab2,Gtk::PACK_SHRINK);
    }
    


    this->show_all();
}

void VersionShower::on_clic(GrpVersion _cpy)
{
    //deja un signal en cour
    if(this->m_onclickLock)
        return ;

    this->m_onclickLock = true;
    this->m_current_sel = _cpy;

    
    //on ajoute les extension existante 
    this->m_ext_sel.remove_all();

    std::vector<std::string> tmp;

    for( const auto & vers : _cpy)
    {
        if(  vers.extension.size() > 0 ) 
        {   
            bool found = false;

            for(const auto & str : tmp)
            {
                if(str == vers.extension)
                {
                    found = true ;
                    break;
                }
            }

            if(!found)
                tmp.push_back( vers.extension );
        }
    }

     for(const auto & str : tmp)
     {
        this->m_ext_sel.append(str);
     }

    
    if(tmp.size() > 0)
    {
         this->m_ext_sel.set_active_text(tmp.front());
         tmp.clear();
    }
   

    
    //ajoute les version existante
    this->m_version_sel.remove_all();
    for( const auto & vers : _cpy)
    {
        if(  vers.version.size() > 0 ) 
        {
            bool found = false;
            for(const auto & str : tmp)
            {
                if(str == vers.version)
                {
                    found = true ;
                    break;
                }
            }

            if(!found)
                tmp.push_back( vers.version );
        }
    }

     for(const auto & str : tmp)
        this->m_version_sel.append(str);
     
     
    if(tmp.size() > 0)
    {
        this->m_version_sel.set_active_text( tmp.back() );
        tmp.clear();
    }
    
    
    //ajoute les partie existante
    this->m_part_sel.remove_all();
    for( const auto & vers : _cpy)
    {
        if(  vers.part.size() > 0 ) 
        {
            bool found = false;
            for(const auto & str : tmp)
            {
                if(str == vers.part)
                {
                    found = true ;
                    break;
                }
            }

            if(!found)
                tmp.push_back( vers.part );
        }
    }

     for(const auto & str : tmp)
        this->m_part_sel.append(str);
     
     

    if(tmp.size() > 0)
    {
        this->m_part_sel.set_active_text( tmp.back() );
        tmp.clear();
    }
    

    //completel'infobare
    

    this->m_autor.set_label( _cpy.get_autor() );
    this->m_createDate.set_label( _cpy.get_createDate() );
    this->m_modifDate.set_label( _cpy.get_createDate() );


    this->update_complet_path();

    gtkmmRemoveChilds(this->m_grpInfoPath);

    for( const auto & vers : this->m_current_sel)
    {
        this->m_grpInfoPath.pack_start(*Gtk::manage( new Gtk::Label(*this->atMainPath()+"\\"+ vers.dir + "\\" + vers.name +"." +vers.extension)),Gtk::PACK_SHRINK);
    }

    this->m_onclickLock = false;

}

void VersionShower::update_complet_path(void)
{
    for(const auto & vers : this->m_current_sel)
    {
        if( this->m_ext_sel.get_active_text() == vers.extension  && this->m_part_sel.get_active_text() == vers.part && this->m_version_sel.get_active_text() == vers.version)
        {
            this->m_complet_path.set_text(*this->atMainPath()+"\\"+ vers.dir + "\\" + vers.name +"." +vers.extension);
            break;
        }
    }
}

//class qui définit un dossier comme conteneur
class Container : public std::vector<GrpVersion> 
{
    public: 
        Container( const std::string & _name);
        const std::string get_name(void)const;
        void addVersionShower(std::shared_ptr<VersionShower> _ptr_vs);
        const std::shared_ptr<VersionShower> & atVersionShower(void);

        void set_authExt(const std::string & _authExt);
        void set_reg_version(const std::string & _str);
        void set_reg_id(const std::string & _str);
        void set_reg_autor(const std::string & _str);
        void set_reg_part(const std::string & _str);

        const std::string get_authExt(void) const;

        const std::string get_reg_version(void) const;
        const std::string get_reg_id(void) const;
        const std::string get_reg_autor(void) const;
        const std::string get_reg_part(void) const;


        void add2GrpVersion(const Version & _v);
    protected:

    const std::string m_name;
    std::shared_ptr<VersionShower> m_ptr_vs;
    std::string m_authext;

    std::string m_reg_version  , m_reg_autor , m_reg_id , m_reg_part;
};

Container::Container( const std::string & _name): std::vector<GrpVersion>() ,  m_name(_name)
{
    this->m_authext = "";
}
const std::string Container::get_name(void)const
{
    return this->m_name;
}

void Container::addVersionShower(std::shared_ptr<VersionShower> _ptr_vs)
{
    this->m_ptr_vs = _ptr_vs;
}
const std::shared_ptr<VersionShower> & Container::atVersionShower(void)
{
    if(!this->m_ptr_vs)
        throw std::logic_error("bad VersionShower acces");

    return this->m_ptr_vs;
}

void  Container::add2GrpVersion(const Version & _v)
{
    for(auto & grp : *this)
    {
        if(grp.get_name() == _v.name)
        {
            grp.push_back(_v);
            return ;
        }
    }

    GrpVersion tmp;
    tmp.push_back(_v);

    this->push_back(tmp);
}

void Container::set_authExt(const std::string & _authExt)
{
    this->m_authext = _authExt;
}
const std::string Container::get_authExt(void) const
{
    return this->m_authext;
}

void Container::set_reg_version(const std::string & _str)
{
    this->m_reg_version = _str;
}
void Container::set_reg_id(const std::string & _str)
{
    this->m_reg_id = _str;
}
void Container::set_reg_autor(const std::string & _str)
{
    this->m_reg_autor = _str;
}
void Container::set_reg_part(const std::string & _str)
{
    this->m_reg_part = _str;
}


const std::string Container::get_reg_version(void) const
{
    return this->m_reg_version;
}
const std::string Container::get_reg_id(void) const
{
    return this->m_reg_id;
}
const std::string Container::get_reg_autor(void) const
{
    return this->m_reg_autor;
}
const std::string Container::get_reg_part(void) const
{
    return this->m_reg_part;
}

std::string regSearch(const std::string & _reg , const std::string & _str)
{
    if( _reg.size() == 0 || _str.size() == 0)
        return "";
    
    std::regex pattern(_reg ,  std::regex_constants::ECMAScript | std::regex_constants::icase);

    std::smatch match;
    if (std::regex_search(_str, match, pattern)) 
    {
        return match.str();
    } 

    return "";

}


//class qui definit un dossier comme materiel
class Device 
{
    public:
        Device( const std::string & _name);
        std::vector<Container> & get_containers(void);
        const std::string get_name(void)const;
        void update( const std::string & _mainPath , const std::vector<std::string> & _files,bool _forceUpdate = false);
    protected:

        std::vector<Container> m_containers;
        const std::string m_name;

        bool m_update;

};

Device::Device( const std::string & _name):m_name(_name),m_update(false)
{
}

std::vector<Container> & Device::get_containers(void)
{
    return this->m_containers;
}

const std::string Device::get_name(void)const
{
    return this->m_name;
}

void Device::update( const std::string & _mainPath , const std::vector<std::string> & _files , bool _forceUpdate)
{

    if( this->m_update && !_forceUpdate)
        return ;

        //part du principe qu'on est pas dans une racite materiel
    std::vector<std::string>  pathDevice ;

    for(auto & f : _files)
    {
        auto tmp = sep_sub_and_name( f );
        
        bool found =false;
        //! ici on cherche u materiel
        
        while ( std::get<0>(tmp).size() > 0)
        {
            tmp = sep_sub_and_name( std::get<0>(tmp) );

            std::string tmpName = std::get<1>(tmp);
            upper(tmpName);
            
            if( tmpName == this->m_name )
            {
                found = true;
                break;
            }
        }
        
        if( found )
        {
                pathDevice.push_back( f.substr(_mainPath.size() , f.size()) );
                
                //supprime la racine si presente
                if(pathDevice.back().front() == '/' || pathDevice.back().front() == '\\')
                    pathDevice.back().erase(pathDevice.back().begin());
        }
           
    }


    //recherche les differant contenaire
    
    for(auto & c : m_containers)
    {
        c.clear();

        for(const auto &  pd : pathDevice)
        {
            std::string tmpStr = pd;

            upper(tmpStr);
            auto found = tmpStr.find( c.get_name() );

            if( found!=std::string::npos)
            {
                
                auto pathName = sep_sub_and_name(pd);
                auto nameExt = sep_name_and_ext(std::get<1>(pathName));
                
                //ajoute si l'extension est supporté
                if( c.get_authExt().find( std::get<1>(nameExt) ) != std::string::npos )
                {
                    Version tmp ;
                    tmp.dir = std::get<0>(pathName);
                    
                    tmp.name = std::get<0>(nameExt);
                    tmp.id = regSearch(c.get_reg_id() , tmp.name);
                    
                    if(tmp.id.size() == 0)
                        tmp.id = tmp.name;

                    tmp.autor =  regSearch(c.get_reg_autor() , tmp.name);
                    tmp.part =  regSearch(c.get_reg_part() , tmp.name);
                    tmp.version =  regSearch(c.get_reg_version() , tmp.name);

                    
                    tmp.extension = std::get<1>(nameExt);

                    std::string tmpFileName =  _mainPath + "\\"+ tmp.dir +"\\"+ tmp.name + "."+ tmp.extension;


                    // Obtenez le temps de dernière modification du fichier
                    std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(tmpFileName);

                    tmp.modifDate = std::string(std::format("{}",lastWriteTime)).substr(0,10) ;
                    tmp.creatDate = get_createDate(tmpFileName);
                    
                    // Convertissez std::time_t en une chaîne de caractères représentant la date et l'heure
                    

                    c.add2GrpVersion(tmp);
                }
                
            }
        } 

    }
    pathDevice.clear();
    pathDevice.shrink_to_fit();
    
    this->m_update  =true;
}

std::string getParamValue( const std::string & _paramName  , std::vector<std::string> & _params , bool _remove_found =false )
{
    std::string res;

    for( auto  it = _params.begin() ; it != _params.end() ; it++)
    {
       if( it->find("=") != std::string::npos )
       {
            auto sep = sep_string<'='>( *it );

            std::string tmpstr = std::get<0>(sep);
            upper(tmpstr);

            if( tmpstr == _paramName )
            {
                res =std::get<1>(sep);

                if(_remove_found)
                    _params.erase(it);

                break; 
            }
       }
    }

    return res;
}

static std::tuple<std::string,ptrVecDevice> load(const std::string & _ressourcePath)
{
    std::clog << "load configuration ...   " << std::endl;
    const auto start{std::chrono::steady_clock::now()};

    ptrVecDevice devices;


    //charge les ficheir de maniere asynchrone // utlise 3 thread
    std::future< std::vector<std::string> > deviceTh = std::async (readList, _ressourcePath + "\\devicesList.csv" , true);
    std::future< std::vector<std::string> > contTh = std::async (readList, _ressourcePath + "\\containerList.csv" , true);
    std::future< std::vector<std::string> > parameterTh = std::async (readList, _ressourcePath + "\\parameters.csv" , false);
    
    std::vector<std::string>&& lsContainer = contTh.get();
    std::vector<std::string>&& lsParameter = parameterTh.get();
    std::vector<std::string>&& lsDevices = deviceTh.get();


    if(  lsContainer.size() == 0)
        throw std::logic_error("no containers found");
    
    if( lsDevices.size() == 0 )
        throw std::logic_error("no devices subpath found");
    
    if(lsParameter.size() == 0)
        throw std::logic_error("no parametres found");


    std::string && mainPath = getParamValue("MAIN_PATH" , lsParameter , true);


    if(mainPath.size() == 0)
        throw std::logic_error("no mainPath  found");

    if(mainPath.back() == '\\' || mainPath.back() == '/')
        mainPath.pop_back();


    devices = ptrVecDevice(new std::vector<Device>());

    //contruis une liste de contenaire patron pour ne pas repeter getParamValue
    std::vector<Container> tmpCont;
    for(const auto & cnt : lsContainer)
    {
        tmpCont.emplace_back( cnt );
        tmpCont.back().set_authExt( getParamValue("AUTHEXT[" + cnt +"]" , lsParameter , true) );

        tmpCont.back().set_reg_version( getParamValue("VERSIONREGEX[" + cnt +"]" , lsParameter , true));
        tmpCont.back().set_reg_id( getParamValue("IDREGEX[" + cnt +"]" , lsParameter , true));
        tmpCont.back().set_reg_autor( getParamValue("AUTORREGEX[" + cnt +"]" , lsParameter , true));
        tmpCont.back().set_reg_part( getParamValue("PARTREGEX[" + cnt +"]" , lsParameter , true));
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

    std::clog <<"succes load in : " << elapsed_seconds.count() * 1000 << " ms" <<std::endl;

    return {mainPath , devices};
}



class AutoCompletVisualiz :  public TemplateGui , public Gtk::ScrolledWindow
{
    public:
         AutoCompletVisualiz(void);

         void update(std::vector<std::string> const & list);

         sigc::signal<void ,std::string> & signal_select(void);

    private:
    Gtk::HBox m_hbox;
    std::vector<Gtk::Button> m_button;

    void signal_clicked(std::string const & value);


    sigc::signal<void , std::string > m_signalSelect;

};
AutoCompletVisualiz::AutoCompletVisualiz(void):TemplateGui(), Gtk::ScrolledWindow()
{
    this->add( this->m_hbox );
}

void AutoCompletVisualiz::signal_clicked(std::string const & value)
{
    this->m_signalSelect.emit(value);
}

 sigc::signal<void ,std::string> & AutoCompletVisualiz::signal_select(void)
 {
    return this->m_signalSelect;
 }

void AutoCompletVisualiz::update(std::vector<std::string> const & list)
{
    for(auto it = this->m_button.begin() ; it != this->m_button.end() ; it++)
    {
        
        this->m_hbox.remove(*it);
    }

    this->m_button.clear();

    for(auto i =0 ; i <  list.size() ;i++)
    {
        this->m_button.push_back( Gtk::Button(list[i]));
        this->m_hbox.pack_start( this->m_button.back() ,  Gtk::PACK_SHRINK);

        this->m_button.back().set_can_focus(false);
        this->m_button.back().set_focus_on_click(false);
        this->m_button.back().signal_clicked().connect( sigc::bind<std::string>(sigc::mem_fun(*this, &AutoCompletVisualiz::signal_clicked),list[i]));
    }

    this->m_hbox.show_all();
}

//

class Prompt  :  public TemplateGui , public Gtk::VBox
{
    public : 

        Prompt(void);

    private:
        void completionList(void);
        void completion(std::string const & value);
        void enterSignal(void);

        int findDualChar(std::string const & line , const char c1 , const char c2);


        Gtk::Entry m_entry;
        AutoCompletVisualiz m_autoCompletion;

};

Prompt::Prompt(void):TemplateGui() ,  Gtk::VBox()
{
    this->m_entry.set_can_focus(true);
    this->m_entry.set_focus_on_click(true);
    this->m_entry.grab_focus();
    this->m_entry.set_alignment(Gtk::ALIGN_START);
    this->m_entry.set_placeholder_text("Entrer un materiel");

    this->m_entry.set_max_length(30);
    this->m_entry.signal_activate().connect(sigc::mem_fun(*this, &Prompt::enterSignal));
    this->m_entry.signal_changed().connect( sigc::mem_fun( *this , &Prompt::completionList ));

    this->m_autoCompletion.signal_select().connect( sigc::mem_fun( *this , &Prompt::completion ));

    this->pack_start(this->m_autoCompletion);
    this->pack_start(this->m_entry);
}

void Prompt::enterSignal(void)
{
    const auto start{std::chrono::steady_clock::now()};


    std::string current = this->m_entry.get_text();

    if(current.size() ==0)
        return;

    
    bool recurs = current.back() == '*' ? true : false;

    if(recurs)
        current.erase(current.end()-1);
    //cherche si l'entrée est valide

    bool forceUpdate = false;

    std::vector<std::string> files ;

    // Utiliser la fonction récursive pour lister les fichiers
    listerFichiers(*this->atMainPath(), files);

    //update
    for(auto & dev : *this->atDevice() )
    {

        if( dev.get_name() ==  current  && !recurs || dev.get_name().find(current) != std::string::npos && recurs)
        {

            dev.update( *this->atMainPath() , files , forceUpdate );

            if(!recurs)
                break;
        }
    }

    //nettoyage des vue
    for( auto & cont : this->atDevice()->front().get_containers() )
        cont.atVersionShower()->clear();
    
    //ajoute les groupe a afficher
    for(auto & dev : *this->atDevice() )
         if( dev.get_name() ==  current  && !recurs || dev.get_name().find(current) != std::string::npos && recurs)
            for( auto & cont : dev.get_containers() )
                 for(auto & grp : cont)
                     cont.atVersionShower()->push_back(grp);

    //mise a jour des vue
    for( auto & cont : this->atDevice()->front().get_containers() )
        cont.atVersionShower()->update_view();


    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};

    std::clog <<"succes load entry in : " << elapsed_seconds.count() * 1000 << " ms" <<std::endl;
    

}

void Prompt::completion(std::string const & value)
{
    std::string _prompt = this->m_entry.get_text();
    std::string last;

     //garde que le dernier segment ( separer par espace )
    for(auto i = _prompt.size() ;i >0 ; i--)
    {
        if( _prompt[i] == ' ')
        {
            last = std::string(_prompt.begin() ,_prompt.begin() + i +1);
            _prompt.erase(_prompt.begin() ,_prompt.begin() + i +1 );
            break;
        }
    }

    bool dot = false;
    unsigned int posLastDot = 0 ;

    for(auto i = 0 ; i < _prompt.size() ; i++)
    {
        if(_prompt[i] == '.')
        {
            dot = true;
            posLastDot = i;
        }
    }

    if(!dot)
        _prompt = "";
    else
    {
        _prompt.erase(_prompt.begin() + posLastDot ,_prompt.end() );
        _prompt += ".";
    }

    this->m_autoCompletion.update({});
    this->m_entry.set_text(last + _prompt + value);
    this->m_entry.activate();

}

void Prompt::completionList(void)
{
    std::string currentText = this->m_entry.get_text(); 

     
    //permission de caractere
    for(std::string::iterator it = currentText.begin() ; it != currentText.end() ; it++ )
    {
        bool dec =  *it >= 0x30 && *it <= 0x39;
        bool maj =  *it >= 0x41 && *it <= 0x5A;
         bool min =  *it >= 0x61 && *it <= 0x7A;
         bool star = *it == '*';
        if( !dec && !maj && !min && !star)
        {
            currentText.erase(it);
            it = currentText.begin() ;
        }

        //upper
        if(min)
        {
            *it = *it - char(32) ;
        }
    }



    this->m_entry.set_text(currentText);

    if( currentText.size() == 0 || currentText.back() =='*')
        return ;

    std::vector<std::string> autoComplet;

    for(auto & device : *this->m_devices)
    {
        std::string tmp  =device.get_name();
        std::size_t found = tmp.find( currentText );
        // si trouver a l'index 0
        if (found!=std::string::npos && found == 0)
        {
            //verifie que pas deja present
             auto it = std::find( autoComplet.begin(),autoComplet.end() ,tmp );

             if(it == autoComplet.end())
                autoComplet.push_back(tmp);
        }
    }

    this->m_autoCompletion.update(autoComplet);

    if(autoComplet.size() == 1)
    {
        this->m_entry.set_text(autoComplet.front());
        this->m_entry.activate();

        return;
    }
}






class WindowMain : public Gtk::Window , public TemplateGui
{
    public:

        WindowMain( const std::string & _absPath );

        void init(void);
        const std::string get_ressourcePath(void)const;
        
    protected :

    private:

        Gtk::HPaned m_utilitysPanel;
        Gtk::VBox m_usersPanel;
        Gtk::Notebook m_nbk;
        Prompt m_prompt;

        std::string m_absPath , m_ressourcePath;

};

WindowMain::WindowMain(const std::string & _absPath) : Gtk::Window() , TemplateGui() 
{
    this->m_absPath = _absPath;
    this->m_parent = this;
    //this->addMainPath( std::shared_ptr< const std::string >(new const std::string(_mainPath)) );

    auto && path = readList(this->m_absPath + "\\ressourcePath.csv" , false);

    if( path.size() == 0 )
         throw std::logic_error("no ressourcePath found");

    this->m_ressourcePath = path.front();
}

const std::string WindowMain::get_ressourcePath(void)const
{
    return this->m_ressourcePath;
}

void WindowMain::init(void)
{
    this->m_prompt.addDevice(this->m_devices);
    this->m_prompt.addMainPath( this->atMainPath() );

    this->set_icon_from_file(  this->m_ressourcePath+ "\\version-control-lrg.png");

    this->unfullscreen();
	this->set_position(Gtk::WIN_POS_CENTER);
	this->resize(640, 480);
    this->maximize();

    this->addCssProvider( Gtk::CssProvider::create() );

    //chargement du fichier css

    std::string const cssFile =  this->m_ressourcePath + "\\style.css" ;

    try
    {
        this->atCssProvider()->load_from_path( cssFile );
    }
    catch(Gtk::CssProviderError const & e)
    {
        std::cerr << "Error : " << cssFile << " cannot be load" << std::endl;
    }

    //class pour fichier css
    this->get_style_context()->add_class("windows");
    this->m_utilitysPanel.get_style_context()->add_class("utilitysPanel");
    this->m_usersPanel.get_style_context()->add_class("userPanel");

    this->add(this->m_utilitysPanel);


    this->m_utilitysPanel.pack1( this->m_usersPanel );

    this->m_utilitysPanel.pack2( *Gtk::manage(new Gtk::Button( "PDF" )) );

    this->m_usersPanel.pack_start(this->m_prompt  , Gtk::PACK_SHRINK);

    if( this->atDevice()->size() > 0 )
    {
        for(auto & cont : this->atDevice()->front().get_containers() )
        {
            cont.addVersionShower( std::shared_ptr< VersionShower>( new VersionShower() ) ) ;
            cont.atVersionShower()->addMainPath( this->atMainPath());
            this->m_nbk.append_page( *cont.atVersionShower() ,  *Gtk::manage(new Gtk::Label( cont.get_name() )) );
        }

        //propage sur tout les device mes shower
        for(auto & dev : *this->atDevice() )
        {
            for(int i = 0 ; i < dev.get_containers().size() ; i++ )
            {
                dev.get_containers().at(i).addVersionShower( this->atDevice()->front().get_containers().at(i).atVersionShower() ) ;
            }
        }
    }

    this->m_nbk.set_scrollable();
    this->m_nbk.popup_enable();
    
    this->m_usersPanel.pack_start( this->m_nbk );
}


int main(int argc, char *argv[]) 
{
    auto progmPath = sep_sub_and_name(std::string(argv[0]));

    Gtk::Main app(argc, argv);

   
    try
    {
         WindowMain fenetre(std::get<0>(progmPath));

	    fenetre.set_title("Maintenance File Grouper Version Tool");

        //! charge les donné , doit etre fait avant WindowMain::init()
        auto tload = load( fenetre.get_ressourcePath() );
        fenetre.addMainPath( std::shared_ptr< const std::string >(new const std::string(std::get<0>(tload))) );
        fenetre.addDevice( std::get<1>(tload) );
        
        fenetre.init();

        fenetre.show_all();
        
        Gtk::Main::run(fenetre);
    }
    catch(std::exception const & _e)
    {
        std::cerr << _e.what() << std::endl;

        std::cin.get();
    }
    
    

    return 0;
}



/*
#include <Windows.h>
#include <Shobjidl.h>
#include <propkey.h>
#include <propvarutil.h>
#include <tuple>
#include <iostream>
#include <string>
#include <strsafe.h>
#include <shobjidl.h>
#include <propsys.h>
#include <propvarutil.h>
#include <propkey.h>
#include <strsafe.h>
#pragma comment(lib, "Propsys.lib")

using FileProperties = std::tuple<std::string, std::string, std::string>;
std::string convert_wstring_to_string(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}
std::wstring getFileProperties(const std::wstring& filePath) {


    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        // Obtenir l'interface IPropertyStore du fichier
        IShellItem2* psi;

        hr = SHCreateItemFromParsingName(filePath.c_str(), NULL, IID_PPV_ARGS(&psi));
        //hr = SHGetPropertyStoreFromParsingName(filePath.c_str(), nullptr, GPS_READWRITE, IID_PPV_ARGS(&pPropertyStore));
        IPropertyDescription* pPropertyDescription = nullptr;
        hr = PSGetPropertyDescription(PKEY_Comment, IID_PPV_ARGS(&pPropertyDescription));
        if (SUCCEEDED(hr)) {
            // Extraire l'auteur
            IPropertyStore* pps;
            PROPERTYKEY authorPropertyKey;
            hr = psi->GetPropertyStore(GPS_DEFAULT , IID_IPropertyStore ,(void**)&pps);

            LPWSTR label;
            pPropertyDescription->GetPropertyKey(&authorPropertyKey);
           
            if (SUCCEEDED(hr)) {
                PROPVARIANT propVar;
                PropVariantInit(&propVar);
            
               // hr = pps->GetValue(PKEY_DateModified, &propVar);
                PSFormatPropertyValue(pps , pPropertyDescription ,PDFF_DEFAULT ,&label );
                
                hr = pps->GetValue(authorPropertyKey, &propVar);
                
                std::cout << convert_wstring_to_string(label) << std::endl;
                

                return std::wstring();
                    
            //PropVariantClear(&propVar);
                
            }

            // Libérer l'interface IPropertyStore
            pps->Release();
        }

        CoUninitialize();
    }

    return L"";
}

int main() {
    // Remplacez le chemin du fichier par le chemin réel de votre fichier
    std::wstring filePath = L"C:\\Users\\Rigel85\\Desktop\\C_test\\2-Ligne2\\Machine20\\1-meca\\10_231028_01_01_JF_01.dft";

 getFileProperties(filePath);
    std::cin.get();
    return 0;
}

*/