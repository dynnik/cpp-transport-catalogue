#pragma once
#include <memory>
#include <variant>

#include "router.h"
#include "transport_catalogue.h"

namespace transport::router
{
    struct RoutingSettings
    {
        int waitTime = 1;
        double velocity = 1;
    };

    struct WaitEdgeInfo
    {
        std::string_view name;
        double time = 0.0;
    };

    struct BusEdgeInfo
    {
        std::string_view name;
        int span_count = 0;
        double time = 0.0;
    };

    struct VertexIds
    {
        graph::VertexId in = 0;
        graph::VertexId out = 0;
    };

    using Time = double;
    using Router = graph::Router<Time>;
    using Graph = graph::DirectedWeightedGraph<Time>;
    using EdgeInfo = std::variant<WaitEdgeInfo, BusEdgeInfo>;
    using RouteInfo = std::pair<double, std::vector<EdgeInfo>>;
    using EdgesInfo = std::unordered_map<graph::EdgeId, EdgeInfo>;
    using StopVertexes = std::unordered_map<std::string_view, VertexIds>;

    class TransportRouter
    {
    private:
        static constexpr Time TO_MINUTES = (3.6 / 60.0);

    public:
        //TransportRouter() = default;

        explicit TransportRouter(const transport_catalogue::TransportCatalogue&, const RoutingSettings&);

        std::optional<RouteInfo> GetRouteInfo(const std::string_view& from, const std::string_view& to) const;

        const EdgesInfo& GetEdgesInfo() const
        {
            return edges_info_;
        }
        const RoutingSettings& GetSettings() const
        {
            return settings_;
        }
        const Graph& GetGraph() const
        {
            return graph_;
        }

        const Router::RoutesInternalData& GetRoutesInternalData() const
        {
            return router_->GetRoutesInternalData();
        }

        const StopVertexes& GetStopsVertexIds() const
        {
            return stops_vertex_ids_;
        }
    private:

        void InitializeVertexes();
        void InitializeEdges();

        const transport_catalogue::TransportCatalogue& catalogue_;
        const RoutingSettings settings_;
        std::unique_ptr<Router> router_;
        StopVertexes stops_vertex_ids_;
        EdgesInfo edges_info_;
        Graph graph_;
    };
}
